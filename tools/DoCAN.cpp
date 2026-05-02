/* **************************************************************************************************** */
/**
 * @file        DoCAN.cpp
 * @author      Alakshendra Singh
 * @brief       Diagonstics on CAN (DoCAN)
 * @version     2.0
 *
 * @copyright   Copyright (c) 2025
 * @note        For Reporting Any Issue Don't Contact Me. Fix Yourself!
 */
/* **************************************************************************************************** */


#include "DoCAN.hpp"

#include <iostream>
#include <stdio.h>
#include <cstdio>
#include <iomanip>
#include <string>
#include <algorithm>


/* **************************************************************************************************** */
/* ---------------------------------------------------------------------------------------------------- */
/**
 * @brief Log Frame Into Logger File
 * @class ISO_DoCAN (Private)
 * @param CANID CAN ID
 * @param Type Frame Type (Enumeration)
 * @param Mode Send / Receive Mode of The Frame
 */
void ISO_DoCAN::LogFrame (uint32_t CANID, PCAN_MessageType Type, char Mode) {
  static constexpr char HEX[] = "0123456789ABCDEF";
  char Message[64];
  char* P = Message;
  int Bracket = 0;
  switch ((CONFIG.MESSAGE[0] >> 4) & 0xF) {
    case 0  : { Bracket = 0; break; }
    case 1  : { Bracket = 1; break; }
    case 2  : { Bracket = 0; break; }
    case 3  : { Bracket = 2; break; }
    default : { Bracket = 7; break; }
  }
  if (Type == PCAN_EXT) {
    *P++ = HEX[(CANID >> 28) & 0xF];
    *P++ = HEX[(CANID >> 24) & 0xF];
    *P++ = HEX[(CANID >> 20) & 0xF];
    *P++ = HEX[(CANID >> 16) & 0xF];
    *P++ = HEX[(CANID >> 12) & 0xF];
    *P++ = HEX[(CANID >>  8) & 0xF];
    *P++ = HEX[(CANID >>  4) & 0xF];
    *P++ = HEX[CANID        & 0xF];
  } else {
    *P++ = HEX[(CANID >> 8) & 0xF];
    *P++ = HEX[(CANID >> 4) & 0xF];
    *P++ = HEX[CANID        & 0xF];
  }
  *P++ = ' ';
  *P++ = ((Mode | 0x20) == 't') ? 'T' : 'R';
  *P++ = 'x'; *P++ = ' ';
  *P++ = ':'; *P++ = ' ';
  *P++ = '[';
  for (int I = 0; I < 8; I++) {
    uint8_t Value = static_cast<uint8_t>(CONFIG.MESSAGE[I]);
    *P++ = HEX[Value >> 4];
    *P++ = HEX[Value & 0xF];
    if (I == Bracket) { *P++ = ']'; }
    *P++ = ' ';
  }
  *P = '\0';
  LOG->AddLog(LOG_UDS, Message);
}
/* **************************************************************************************************** */

/* **************************************************************************************************** */
/* ---------------------------------------------------------------------------------------------------- */
/**
 * @brief Send Raw Data on CAN via PCAN
 * @class ISO_DoCAN (Private)
 * @param MSG PCAN Data Structure
 * @return (int) Active Low Flag
 * @retval Zero (0) All OK & Non-Zero Means Issue
 */
int ISO_DoCAN::SendRawData (PCAN_Message MSG) {
  return PCAN.Write (MSG);
}
/* **************************************************************************************************** */

/* **************************************************************************************************** */
/* ---------------------------------------------------------------------------------------------------- */
/**
 * @brief Send Internal DoCAN Data on CAN via PCAN
 * @class ISO_DoCAN (Private)
 * @param IsFunctional Is Functional
 * @return (int) Active Low Flag
 * @retval Zero (0) All OK & Non-Zero Means Issue
 */
int ISO_DoCAN::SendData (bool IsFunctional) {
  PCAN_Message MSG;
  if (IsFunctional) {
    MSG.TYPE = CONFIG.CANID.FUN.TYPE;
    MSG.ID = CONFIG.CANID.FUN.ID;
  } else {
    MSG.TYPE = CONFIG.CANID.TX.TYPE;
    MSG.ID = CONFIG.CANID.TX.ID;
  }
  MSG.LEN = 8;
  for (int I = 0; I < 8; I++) {
    MSG.DATA[I] = CONFIG.MESSAGE[I];
  }
  LogFrame (MSG.ID, MSG.TYPE, 'T');
  return PCAN.Write (MSG);
}
/* **************************************************************************************************** */

/* **************************************************************************************************** */
/* ---------------------------------------------------------------------------------------------------- */
/**
 * @brief Transmit Flow Control Frame
 * @class ISO_DoCAN (Private)
 * @param Abort Abort Receive
 * @return (int) Active Low Flag
 * @retval Zero (0) All OK & Non-Zero Means Issue
 */
int ISO_DoCAN::TransmitFlowControl (const bool Abort) {
  PCAN_Message MSG;
  MSG.LEN = 8;
  if (CONFIG.RX.ISFUNCTIONAL) {
    MSG.TYPE = CONFIG.CANID.FUN.TYPE;
    MSG.ID = CONFIG.CANID.FUN.ID;
  } else {
    MSG.TYPE = CONFIG.CANID.TX.TYPE;
    MSG.ID = CONFIG.CANID.TX.ID;
  }
  for (int I = 3; I < 8; I++) {
    MSG.DATA[I] = CONFIG.SETTINGS.PADDING;
  }
  if (Abort) {
    MSG.DATA[0] = 0x32;
    MSG.DATA[1] = 0x00;
    MSG.DATA[2] = 0x00;
  } else {
    MSG.DATA[0] = 0x30;
    MSG.DATA[1] = 0x00;
    MSG.DATA[2] = CONFIG.SETTINGS.STMIN;
  }
  LogFrame (MSG.ID, MSG.TYPE, 'T');
  return PCAN.Write (MSG);
}
/* **************************************************************************************************** */

/* **************************************************************************************************** */
/* ---------------------------------------------------------------------------------------------------- */
/**
 * @brief Receive Flow Control Frame
 * @class ISO_DoCAN (Private)
 * @param BS Block Size
 * @param STMin Time Between Frames
 * @return (int) Active Low Flag
 * @retval Zero (0) All OK & Non-Zero Means Issue
 */
int ISO_DoCAN::ReceiveFlowControl (uint8_t &BS, uint8_t &STMin) {
  uint64_t OverallTimeout = CONFIG.SETTINGS.TIMEOUT << 4;
  uint64_t OverallTime = Time.MicroClock();
  uint64_t CycleTime = Time.MicroClock();
  PCAN_Message MSG;
  do {
    if (PCAN.Read (MSG) == PCAN_ERROR_OK) {
      if ((MSG.TYPE == CONFIG.CANID.RX.TYPE) && (MSG.ID == CONFIG.CANID.RX.ID) && (MSG.LEN == 8)) {
        LogFrame (MSG.ID, MSG.TYPE, 'R');
        if (MSG.DATA[0] == 0x30) {
          BS = MSG.DATA[1];
          STMin = MSG.DATA[2];
          return 0;
        } else if (MSG.DATA[0] == 0x31) { // FS : Wait
          CycleTime = Time.MicroClock();
        }
        else if (MSG.DATA[0] == 0x32) { // FS : Abort
          return 1;
        }
      }
    }
  } while (((Time.MicroClock() - CycleTime) < CONFIG.SETTINGS.TIMEOUT) &&
      ((Time.MicroClock() - OverallTime) < OverallTimeout));
  return 1;
}
/* **************************************************************************************************** */

/* **************************************************************************************************** */
/* ---------------------------------------------------------------------------------------------------- */
/**
 * @brief Receive Single Frame According to ISO 14229-3
 * @class ISO_DoCAN (Private)
 * @return (int) Active Low Flag
 * @retval Zero (0) All OK & Non-Zero Means Issue
 */
int ISO_DoCAN::FrameRX_SF (void) {
  int Length = CONFIG.MESSAGE[0] & 0x0F;
  if ((Length < 8) && (Length != 0)) {
    CONFIG.RX.DONE = 0;
    *CONFIG.BUFFER.LEN = Length;
    for (int I = 0; I < Length; I++) {
      CONFIG.BUFFER.DATA[I] = CONFIG.MESSAGE[I+1];
    }
    CONFIG.RX.DONE = 1;
    return 0;
  } else {
    CONFIG.ERRORCODE = DoCAN_ERR_FRAMEISSUE;
    CONFIG.RX.DONE = 0;
    return 1;
  }
}
/* **************************************************************************************************** */

/* **************************************************************************************************** */
/* ---------------------------------------------------------------------------------------------------- */
/**
 * @brief Receive First Frame According to ISO 14229-3
 * @class ISO_DoCAN (Private)
 * @return (int) Active Low Flag
 * @retval Zero (0) All OK & Non-Zero Means Issue
 */
int ISO_DoCAN::FrameRX_FF (void) {
  const int Length = (uint16_t)( ((CONFIG.MESSAGE[0] & 0x0F) << 8) | CONFIG.MESSAGE[1] );
  if ((Length <= CONFIG.BUFFER.MAXLENGTH) && (Length > 7)) {
    CONFIG.RX.LENGTH = Length;
    CONFIG.RX.FRAMES = 0;
    CONFIG.RX.INDEX = 0;
    CONFIG.RX.DONE = 0;

    *CONFIG.BUFFER.LEN = Length;
    for (int I = 2; I < 8; I++) {
      CONFIG.BUFFER.DATA[CONFIG.RX.INDEX] = CONFIG.MESSAGE[I];
      CONFIG.RX.INDEX++;
    }

    if ( TransmitFlowControl (false) ) {
      CONFIG.ERRORCODE = DoCAN_ERR_INTERNALISSUE;
      CONFIG.RX.DONE = 0;
      return 1;
    }
    CONFIG.RX.TIME = Time.MicroClock();
    return 0;
  } else {
    CONFIG.ERRORCODE = DoCAN_ERR_FRAMEISSUE;
    CONFIG.RX.DONE = 0;
    if ( TransmitFlowControl (true) ) {
      CONFIG.ERRORCODE = DoCAN_ERR_INTERNALISSUE;
    }
    return 1;
  }
}
/* **************************************************************************************************** */

/* **************************************************************************************************** */
/* ---------------------------------------------------------------------------------------------------- */
/**
 * @brief Receive Consecutive Frame According to ISO 14229-3
 * @class ISO_DoCAN (Private)
 * @return (int) Active Low Flag
 * @retval Zero (0) All OK & Non-Zero Means Issue
 */
int ISO_DoCAN::FrameRX_CF (void) {
  if ( (CONFIG.MESSAGE[0] & 0x0F) == CONFIG.RX.FRAMES ) {
    CONFIG.RX.FRAMES = (CONFIG.RX.FRAMES + 1) % 16;
    CONFIG.RX.TIME = Time.MicroClock();
    for (int I = 1; I < 8; I++) {
      CONFIG.BUFFER.DATA[CONFIG.RX.INDEX] = CONFIG.MESSAGE[I];
      CONFIG.RX.INDEX++;
      if (CONFIG.RX.INDEX >= CONFIG.RX.LENGTH) {
        CONFIG.RX.DONE = 1;
        return 0;
      }
    }
    return 0;
  } else {
    CONFIG.ERRORCODE = DoCAN_ERR_WRONGFRAMECOUNTER;
    CONFIG.RX.DONE = 0;
    return 1;
  }
}
/* **************************************************************************************************** */

/* **************************************************************************************************** */
/* ---------------------------------------------------------------------------------------------------- */
/**
 * @brief Transmit Single Frame According to ISO 14229-3
 * @param IsFunctional Is Functional
 * @return (int) Active Low Flag
 * @retval Zero (0) All OK & Non-Zero Means Issue
 */
int ISO_DoCAN::FrameTX_SF (void) {
  CONFIG.MESSAGE[0] = 0x0F & (CONFIG.TX.LENGTH);
  int I = 1;
  while (CONFIG.TX.INDEX < CONFIG.TX.LENGTH) {
    CONFIG.MESSAGE[I]  = CONFIG.BUFFER.DATA[CONFIG.TX.INDEX];
    CONFIG.TX.INDEX++;
    I++;
  }
  while (I < 8) {
    CONFIG.MESSAGE[I] = CONFIG.SETTINGS.PADDING;
    I++;
  }
  CONFIG.TX.DONE = 1;
  return SendData (CONFIG.TX.ISFUNCTIONAL);
}
/* **************************************************************************************************** */

/* **************************************************************************************************** */
/* ---------------------------------------------------------------------------------------------------- */
/**
 * @brief Transmit First Frame According to ISO 14229-3
 * @param IsFunctional Is Functional
 * @return (int) Active Low Flag
 * @retval Zero (0) All OK & Non-Zero Means Issue
 */
int ISO_DoCAN::FrameTX_FF (void) {
  CONFIG.MESSAGE[0] = ((CONFIG.TX.LENGTH >> 8) & 0x0F) | 0x10;
  CONFIG.MESSAGE[1] = (uint8_t)(CONFIG.TX.LENGTH & 0x00FF);
  int I = 2;
  while (I < 8) {
    CONFIG.MESSAGE[I] = CONFIG.BUFFER.DATA[CONFIG.TX.INDEX];
    CONFIG.TX.INDEX++;
    I++;
  }
  CONFIG.TX.DONE = 0;
  return SendData (CONFIG.TX.ISFUNCTIONAL);
}
/* **************************************************************************************************** */

/* **************************************************************************************************** */
/* ---------------------------------------------------------------------------------------------------- */
/**
 * @brief Transmit Consecutive Frame According to ISO 14229-3
 * @param IsFunctional Is Functional
 * @return (int) Active Low Flag
 * @retval Zero (0) All OK & Non-Zero Means Issue
 */
int ISO_DoCAN::FrameTX_CF (void) {
  CONFIG.MESSAGE[0] = 0x20 | CONFIG.TX.FRAMES;
  CONFIG.TX.FRAMES = (CONFIG.TX.FRAMES + 1) % 16;
  int I = 1;
  while (CONFIG.TX.INDEX < CONFIG.TX.LENGTH) {
    CONFIG.MESSAGE[I]  = CONFIG.BUFFER.DATA[CONFIG.TX.INDEX];
    CONFIG.TX.INDEX++;
    I++;
  }
  while (I < 8) {
    CONFIG.MESSAGE[I] = CONFIG.SETTINGS.PADDING;
    I++;
  }
  CONFIG.TX.DONE = (CONFIG.TX.INDEX >= CONFIG.TX.LENGTH) ? 1 : 0;
  return SendData (CONFIG.TX.ISFUNCTIONAL);
}
/* **************************************************************************************************** */

/* **************************************************************************************************** */
/* ---------------------------------------------------------------------------------------------------- */
/**
 * @brief Receive DoCAN Frame According to ISO 14229-3
 * @class ISO_DoCAN (Public)
 * @param IsFunctional Is Functional
 * @return (int) Active Low Flag
 * @retval Zero (0) All OK & Non-Zero Means Issue
 */
int ISO_DoCAN::Receive (bool IsFunctional) {
  uint64_t OverallTimeout = CONFIG.SETTINGS.TIMEOUT << 4;
  uint64_t OverallTime = Time.MicroClock();
  CONFIG.RX.TIME = Time.MicroClock();
  PCAN_Message MSG;
  CONFIG.RX.ISFUNCTIONAL = IsFunctional;
  bool MultiFrame = false;

  do {
    if (PCAN.Read (MSG) == PCAN_ERROR_OK) {
      if ((MSG.TYPE == CONFIG.CANID.RX.TYPE) && (MSG.ID == CONFIG.CANID.RX.ID) && (MSG.LEN == 8)) {
        LogFrame (MSG.ID, MSG.TYPE, 'R');
        uint8_t PCI = (MSG.DATA[0] & 0xF0) >> 4;
        if ( (PCI != 0) && (PCI != 1) && (PCI != 2) ) { continue; }

        if ((MSG.DATA[0] == 0x03) && (MSG.DATA[1] == 0x7F) && (MSG.DATA[3] == 0x78)) {
          CONFIG.RX.TIME = Time.MicroClock();
          continue;
        }

        for (int I = 0; I < 8; I++) { CONFIG.MESSAGE[I] = MSG.DATA[I]; }
        *CONFIG.BUFFER.CANID = MSG.ID;
        CONFIG.RX.DONE = 0;

        int Status = 0;
        if (PCI == 0) {             // Single Frame
          if (MultiFrame) { continue; }
          Status = FrameRX_SF();
        }
        else if (PCI == 1) {        // First Frame
          if (MultiFrame) { continue; }
          Status = FrameRX_FF();
          MultiFrame = true;
        } else {                    // Consecutive Frame
          Status = FrameRX_CF();
        }

        if (Status != 0) {
            return 1; // Processing Error
        }
        if (CONFIG.RX.DONE == 1) {
            return 0; // Reception Completed
        }

      }
    }
  } while (((Time.MicroClock() - CONFIG.RX.TIME) < CONFIG.SETTINGS.TIMEOUT) &&
      ((Time.MicroClock() - OverallTime) < OverallTimeout));
  return 2; // Timed Out
}
/* **************************************************************************************************** */

/* **************************************************************************************************** */
/* ---------------------------------------------------------------------------------------------------- */
/**
 * @brief Write DoCAN Frame According to ISO 14229-3
 * @class ISO_DoCAN (Public)
 * @param IsFunctional Is Functional
 * @return (int) Active Low Flag
 * @retval Zero (0) All OK & Non-Zero Means Issue
 * @retval 1 : Internal Issue
 * @retval 2 : Wrong Value
 */
int ISO_DoCAN::Write (bool IsFunctional) {
  int Length = *CONFIG.BUFFER.LEN;
  if ((Length > CONFIG.BUFFER.MAXLENGTH) || (Length == 0)) { return 2; }
  CONFIG.TX.ISFUNCTIONAL = IsFunctional;
  CONFIG.TX.LENGTH = Length;
  CONFIG.TX.FRAMES = 0;
  CONFIG.TX.INDEX = 0;
  CONFIG.TX.DONE = 0;

  if (Length < 8) { // Single Frame Send
    return FrameTX_SF();
  }
  else {            // Multi Frame Send
    int Status = FrameTX_FF();
    if (Status != 0) { return Status; }
    do {

      uint8_t BS = 0, STMin = 0;
      uint64_t TimeBetweenFrame = 0;
      Status = ReceiveFlowControl (BS, STMin);
      if (Status != 0) { return Status; }

      if (STMin <= 0x7F) {
        TimeBetweenFrame = STMin * 1000;
      } else if ((STMin <= 0xF9) && (STMin >= 0xF1)) {
        TimeBetweenFrame = (STMin & 0x0F) * 100;
      } else {
        return 2;
      }

      if (BS == 0) {
        while (CONFIG.TX.DONE != 1) {
          Time.MicroDelay (TimeBetweenFrame);
          Status = FrameTX_CF();
          if (Status != 0) { return Status; }
        }
        return 0;
      }
      else {
        int I = 0;
        while ((I < BS) && (CONFIG.TX.DONE != 1)) {
          Time.MicroDelay (TimeBetweenFrame);
          Status = FrameTX_CF();
          if (Status != 0) { return Status; }
          I++;
        }
      }

    } while(CONFIG.TX.DONE != 1);

    if (CONFIG.TX.DONE == 1) {
      return 0;
    } return 1;
  }
}
/* **************************************************************************************************** */

/* **************************************************************************************************** */
/* ---------------------------------------------------------------------------------------------------- */
/**
 * @brief Request And Response Transfered Via DoCAN According to ISO 14229-3
 * @class ISO_DoCAN (Public)
 * @param IsFunctional Is Functional
 * @return (int) Active Low Flag
 * @retval Zero (0) All OK & Non-Zero Means Issue
 */
int ISO_DoCAN::Transfer (bool IsFunctional = false) {
  CONFIG.ERRORCODE = DoCAN_ERR_OK;
  int Status = 0;
  PCAN.Reset();
  Status = Write (IsFunctional);
  if (Status != 0) {
    return Status;
  }
  Status = Receive (IsFunctional);
  if (Status != 0) {
    return Status;
  }
  return 0;
}
/* **************************************************************************************************** */

/* **************************************************************************************************** */
/* ---------------------------------------------------------------------------------------------------- */
/**
 * @brief Configures The Logger Instance Used By The DoCAN Layer
 * @class ISO_DoCAN (Public)
 * @param LoggerInstance Logger Instance Pointer
 * @return int 
 */
int ISO_DoCAN::SetLogger (Logger* LoggerInstance) {
  if (LoggerInstance == nullptr) { return 1; }
  LOG = LoggerInstance;
  return 0;
}
/* **************************************************************************************************** */

/* **************************************************************************************************** */
/* ---------------------------------------------------------------------------------------------------- */
/**
 * @brief Configure Settings for Transport Layer Dynamically
 * @class ISO_DoCAN (Public)
 * @param Timeout Timeout in Microseconds
 * @param Padding Padding For Transmitting Messages
 * @param STMin STMin as per ISO 14229-3
 * @return (int) Active Low Flag
 */
int ISO_DoCAN::SetSettings (uint64_t Timeout, uint8_t Padding, uint8_t STMin) {
  CONFIG.SETTINGS.TIMEOUT = Timeout;
  CONFIG.SETTINGS.PADDING = Padding;
  CONFIG.SETTINGS.STMIN = STMin;
  return 0;
}
/* **************************************************************************************************** */

/* **************************************************************************************************** */
/* ---------------------------------------------------------------------------------------------------- */
/**
 * @brief Configure Buffer Pointers And Length
 * @class ISO_DoCAN (Public)
 * @param CANID CANID Receive Pointer
 * @param LEN Data Buffer Length Receive Pointer
 * @param DATA Data Buffer Pointer
 * @param MaxLength Data Buffer Length
 * @return (int) Active Low Flag 
 */
int ISO_DoCAN::SetBuffer (uint32_t* CANID, uint16_t* LEN, uint8_t* DATA, uint16_t MaxLength) {
  if ((CANID == nullptr) || (LEN == nullptr) || (DATA == nullptr) || (MaxLength == 0)) {
    return 1;
  }
  CONFIG.BUFFER.CANID = CANID;
  CONFIG.BUFFER.LEN = LEN;
  CONFIG.BUFFER.DATA = DATA;
  CONFIG.BUFFER.MAXLENGTH = MaxLength;
  return 0;
}
/* **************************************************************************************************** */

/* **************************************************************************************************** */
/* ---------------------------------------------------------------------------------------------------- */
/**
 * @brief Configure CAN IDs for Transport Layer Dynamically
 * @class ISO_DoCAN (Public)
 * @param TXID Transmit CAN ID
 * @param TypeTX Transmit CAN Frame Type
 * @param RXID Receive CAN ID
 * @param TypeRX Receive CAN Frame Type
 * @param FUNID Functional CAN ID
 * @param TypeFUN Functional CAN Frame Type
 * @return (int) Active Low Flag 
 */
int ISO_DoCAN::SetCANIDs (uint32_t TXID, PCAN_MessageType TypeTX, uint32_t RXID, PCAN_MessageType TypeRX, 
    uint32_t FUNID, PCAN_MessageType TypeFUN) {
  if ((FUNID > 0x7FF) && (TypeFUN == PCAN_STD)) { return 1; }
  if ((TXID > 0x7FF) && (TypeTX == PCAN_STD)) { return 1; }
  if ((RXID > 0x7FF) && (TypeRX == PCAN_STD)) { return 1; }
  if (FUNID > 0x1FFFFFFF) { return 1; }
  if (TXID > 0x1FFFFFFF) { return 1; }
  if (RXID > 0x1FFFFFFF) { return 1; }

  CONFIG.CANID.FUN.TYPE = TypeFUN;
  CONFIG.CANID.FUN.ID = FUNID;
  CONFIG.CANID.TX.TYPE = TypeTX;
  CONFIG.CANID.TX.ID = TXID;
  CONFIG.CANID.RX.TYPE = TypeRX;
  CONFIG.CANID.RX.ID = RXID;

  uint32_t MAX = std::max({CONFIG.CANID.FUN.ID, CONFIG.CANID.TX.ID, CONFIG.CANID.RX.ID});
  uint32_t MIN = std::min({CONFIG.CANID.FUN.ID, CONFIG.CANID.TX.ID, CONFIG.CANID.RX.ID});
  PCAN_MessageType Type = PCAN_STD;
  if ((CONFIG.CANID.TX.TYPE == PCAN_EXT) || (CONFIG.CANID.RX.TYPE == PCAN_EXT) || 
      (CONFIG.CANID.FUN.TYPE == PCAN_EXT)) {
    Type = PCAN_EXT;
  }
  if (PCAN.Filter (MIN, MAX, Type) != PCAN_ERROR_OK) {
    return 2;
  }
  return 0;
}
/* **************************************************************************************************** */

/* **************************************************************************************************** */
/* ---------------------------------------------------------------------------------------------------- */
/**
 * @brief Focus Hardware Filter on Receive CAN ID Only
 * @class ISO_DoCAN (Public)
 * @return (int) Active Low Flag 
 */
int ISO_DoCAN::FocusRX (void) {
  if ( PCAN.Filter (CONFIG.CANID.RX.ID, CONFIG.CANID.RX.TYPE) != PCAN_ERROR_OK) {
    return 1;
  } return 0;
}
/* **************************************************************************************************** */

/* **************************************************************************************************** */
/* ---------------------------------------------------------------------------------------------------- */
/**
 * @brief Get DoCAN Error Code Name
 * @class ISO_DoCAN (Public)
 * @return (std::string) Error Code
 */
std::string ISO_DoCAN::GetErrorName (void) {
  switch (CONFIG.ERRORCODE) {
    case DoCAN_ERR_OK                 : { return "ALL OK"; }
    case DoCAN_ERR_INTERNALISSUE      : { return "INTERNAL ISSUE"; }
    case DoCAN_ERR_FRAMEISSUE         : { return "FRAME ISSUE"; }
    case DoCAN_ERR_WRONGFRAMECOUNTER  : { return "WRONG FRAMECOUNTER"; }
    case DoCAN_ERR_DRIVERFAILURE      : { return "DRIVER FAILURE"; }
    case DoCAN_ERR_WRONGDATA          : { return "WRONG DATA"; }
    default                           : { return "UNEXPECTED FAILURE"; }
  }
}
/* **************************************************************************************************** */

/* **************************************************************************************************** */
/* ---------------------------------------------------------------------------------------------------- */
/**
 * @brief Get DoCAN Error Code
 * @class ISO_DoCAN (Public)
 * @return (DoCAN_ERROR) Error Code
 */
DoCAN_ERROR ISO_DoCAN::GetErrorCode (void) {
  return CONFIG.ERRORCODE;
}
/* **************************************************************************************************** */

/* **************************************************************************************************** */
/* ---------------------------------------------------------------------------------------------------- */
/**
 * @brief Start DoCAN Driver And Start DoCAN Layer
 * @class ISO_DoCAN (Public)
 * @param Config DoCAN Configuration Structure
 * @return (DoCAN_ERROR) Error Code
 */
DoCAN_ERROR ISO_DoCAN::Start (DoCAN_ConfigureStructure Setting) {
  PCAN.Uninitialize();
  
  if ( SetSettings (Setting.SETTINGS.TIMEOUT, Setting.SETTINGS.PADDING, Setting.SETTINGS.STMIN) != 0) 
    { return DoCAN_ERR_WRONGDATA; }
  if ( SetBuffer (Setting.BUFFER.CANID, Setting.BUFFER.LEN, Setting.BUFFER.DATA, 
      Setting.BUFFER.MAXLENGTH) ) 
    { return DoCAN_ERR_WRONGDATA; }
  
  CONFIG.SETTINGS.SPEED = Setting.SETTINGS.SPEED;
  if ( PCAN.Initialize (CONFIG.SETTINGS.SPEED) != PCAN_ERROR_OK) 
    { return DoCAN_ERR_DRIVERFAILURE; }
  
  int Status = SetCANIDs (Setting.CANID.TX.ID, Setting.CANID.TX.TYPE, Setting.CANID.RX.ID, 
      Setting.CANID.RX.TYPE, Setting.CANID.FUN.ID, Setting.CANID.FUN.TYPE);
  if (Status == 1)      { return DoCAN_ERR_WRONGDATA; }
  else if (Status == 2) { return DoCAN_ERR_DRIVERFAILURE; }

  CONFIG.ERRORCODE = DoCAN_ERR_OK;
  return DoCAN_ERR_OK;
}
/* **************************************************************************************************** */

/* **************************************************************************************************** */
/* ---------------------------------------------------------------------------------------------------- */
/**
 * @brief Stop DoCAN Driver
 * @class ISO_DoCAN (Public)
 */
void ISO_DoCAN::Stop (void) {
  PCAN.Uninitialize();
  return;
}
/* **************************************************************************************************** */