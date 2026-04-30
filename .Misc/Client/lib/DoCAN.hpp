/* ==================================================================================================== */
/*
 *  DoCAN.h
 *  Diagonostics on CAN (DoCAN)
 *    ISO: 14229 Part 3   - UDSonCAN (DoCAN)
 *  Version: v1.0:0
 *  Developed By: Alakshendra Singh
 *  For Reporting Any Issue Don't Contact Me. Fix Yourself
 */
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @file        DoCAN.h
 * @author      Alakshendra Singh
 * @brief       Diagonstics on CAN (DoCAN)
 * @version     1.0
 *
 * @copyright   Copyright (c) 2025
 */
/* ==================================================================================================== */

#ifndef _DoCAN
#define _DoCAN

#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include <cstdio>
#include <iomanip>
#include "DriverPCAN.hpp"
#include "UDS.hpp"
#include <chrono>

using Clock = std::chrono::steady_clock;

#define DoCAN_ERR_FRAMEOK                 0
#define DoCAN_ERR_FRAMEISSUE              1
#define DoCAN_ERR_FRAMEOVERFLOW           2
#define DoCAN_ERR_WRONGCANID              3
#define DoCAN_ERR_WRONGFRAMECOUNTER       4
#define DoCAN_ERR_DRIVERFAILER            5
#define DoCAN_ERR_INTERNALISSUE           6

#define DoCAN_RX_WORKING                  0
#define DoCAN_RX_IDLE                     1
#define DoCAN_RX_COMPLETE                 2
#define DoCAN_RX_ERROR                    3



/* ==================================================================================================== */
/**
 * @class       DriverPCAN
 * @brief       PCAN Driver For CAN According To ISO 11898-1
 */
/* ---------------------------------------------------------------------------------------------------- */
class ISO_DoCAN {

  protected :

    std::chrono::_V2::steady_clock::time_point StartTime;
    DriverPCAN PCAN;

    struct DoCANMESSAGE {
      uint32_t ID;
      uint8_t LEN;
      uint8_t TYPE;
      uint8_t DATA[8];
    }; DoCANMESSAGE MESSAGE;

    enum DoCANStatus {
      DoCAN_Idle = 0,
      DoCAN_Receive = 1,
      DoCAN_Transmit = 2,
      DoCAN_Wait = 3,
      DoCAN_PHY = 0,
      DoCAN_FUN = 1,
    };

  private :

    struct DoCANCONFIGURATION {
      uint32_t CANID_TX;
      uint32_t CANID_RX;
      uint32_t CANID_FN;
      uint8_t PADDING;
      uint8_t STMIN;
      uint8_t BLOCKS;
      uint8_t * DATA;
      uint16_t * LEN;
      uint32_t * ID;
      uint16_t LENGTH;
      uint64_t P2;
      uint64_t P2_Star;
      uint8_t ERRORCODE;
      uint8_t INIT;
    }; DoCANCONFIGURATION CONFIG;

    struct DoCANSETTINGTX {
      DoCANStatus STATUS;
      uint64_t TIME;
      uint8_t MODE;
      uint64_t STMIN;
      uint8_t BLOCKS;
      uint8_t TXFLAG;

    }; DoCANSETTINGTX SETTINGS_TX;

    struct DoCANSETTINGRX {
      DoCANStatus STATUS;
      uint64_t TIME;
      uint8_t MODE;
      uint16_t LENGTH;
      uint16_t FRAMES;
      uint16_t COUNTER;
      uint8_t BLOCKCOUNTER;
      uint16_t INDEX;
      uint8_t RXFLAG;
    }; DoCANSETTINGRX SETTINGS_RX;

    void FrameTX_FC (uint8_t FS);

    void FrameRX_FC (void);
    void FrameRX_SF (void);
    void FrameRX_FF (void);
    void FrameRX_CF (void);
    void DoCAN_RX (void);


    
    void DoCAN_TX (void);

  public :

    ISO_DoCAN (void) {
      StartTime = Clock::now();
      cout << "\nDoCAN Driver Loaded";
      CONFIG.PADDING = 0x00; CONFIG.STMIN = 0x00; CONFIG.BLOCKS = 0x00; CONFIG.LENGTH = 4095;
      SETTINGS_RX.RXFLAG = 1; SETTINGS_TX.TXFLAG = 1;
      SETTINGS_RX.INDEX = 0; SETTINGS_RX.BLOCKCOUNTER = 0; SETTINGS_RX.COUNTER = 0; SETTINGS_RX.FRAMES = 0;
      SETTINGS_RX.MODE = 0;
      CONFIG.ERRORCODE = 0;
      SETTINGS_RX.STATUS = DoCAN_Idle;
      CONFIG.INIT = 0x00;
    }
    ~ISO_DoCAN (void) {
      cout << "\nDoCAN Driver Unloaded";
    }

    void SetCANID (uint32_t ID_TX, uint32_t ID_RX, uint32_t ID_FN);
    void SetBuffer (uint32_t * ID, uint16_t * Len, uint8_t * Data);
    void SetTiming (uint64_t P2, uint64_t P2_Star);
    void SetUDSParameter (uint8_t Padding, uint32_t STMin, uint8_t Block, uint16_t Length);
    void Start (void);

    uint64_t Clock (uint8_t Mode = 0);
    uint64_t MicroClock (void);
    uint64_t MilliClock (void);
    void Delay (uint64_t Time, uint8_t Mode = 0);
    void MilliDelay (uint64_t Time);
    void MicroDelay (uint64_t Time);

    uint8_t Receive (uint8_t Mode = 0, uint8_t Time = 0);

    

};
/* ==================================================================================================== */


/* ==================================================================================================== */
/**
 * @name        FrameRX_FC
 * @class       ISO_DoCAN (Private)
 * @brief       Receive Flow Control Frame
 * @param []    Nothing
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_DoCAN::FrameRX_FC (void) {
  if ((SETTINGS_RX.STATUS == DoCAN_Wait)) {
    uint8_t FS = MESSAGE.DATA[0] & 0x0F;
    switch (FS) {
        case 0 : { // FS : Continue To Send (CTS)
          SETTINGS_RX.STATUS = DoCAN_Transmit;
          break;
        }
        case 1 : { // FS = Wait (WT)
          SETTINGS_RX.STATUS = DoCAN_Wait;
          SETTINGS_RX.TIME = MicroClock();
          return;
        }
        case 2 : { // FS = Overflow Flag (OF)
          CONFIG.ERRORCODE = DoCAN_ERR_FRAMEOVERFLOW;
          SETTINGS_RX.STATUS = DoCAN_Idle;
          SETTINGS_RX.RXFLAG = DoCAN_RX_ERROR;
          return;
        }
        default : {
          CONFIG.ERRORCODE = DoCAN_ERR_FRAMEISSUE;
          SETTINGS_RX.STATUS = DoCAN_Idle;
          SETTINGS_RX.RXFLAG = DoCAN_RX_ERROR;
          return;
        }
    }
    CONFIG.BLOCKS = MESSAGE.DATA[1];
    if ( MESSAGE.DATA[2] <= 0x7F ) {
      SETTINGS_TX.STMIN = MESSAGE.DATA[2] * 1000;
      SETTINGS_TX.TIME = MicroClock();
      SETTINGS_RX.RXFLAG = DoCAN_RX_COMPLETE;
    } else if (( MESSAGE.DATA[2] <= 0xF9 ) && ( MESSAGE.DATA[2] >= 0xF1 )) {
      SETTINGS_TX.STMIN = (MESSAGE.DATA[2] & 0x0F) * 100;
      SETTINGS_TX.TIME = MicroClock();
      SETTINGS_RX.RXFLAG = DoCAN_RX_COMPLETE;
    } else {
      CONFIG.ERRORCODE = DoCAN_ERR_FRAMEISSUE;
      SETTINGS_RX.STATUS = DoCAN_Idle;
      SETTINGS_RX.RXFLAG = DoCAN_RX_ERROR;
    } return;
  }
  else {
    CONFIG.ERRORCODE = DoCAN_ERR_FRAMEISSUE;
    SETTINGS_RX.STATUS = DoCAN_Idle;
    SETTINGS_RX.RXFLAG = DoCAN_RX_ERROR;
    return;
  }
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        FrameRX_SF
 * @class       ISO_DoCAN (Private)
 * @brief       Receive Single Frame
 * @param []    Nothing
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_DoCAN::FrameRX_SF (void) {
  if (SETTINGS_RX.STATUS == DoCAN_Receive) {
    uint8_t LEN = MESSAGE.DATA[0] & 0x0F;
    if ( LEN <= 7 ) {
      *CONFIG.LEN = LEN;
      *CONFIG.ID = MESSAGE.ID;
      for (uint8_t I = 0; I < LEN; I++) {
        CONFIG.DATA[I] = MESSAGE.DATA[ I + 1 ];
      }
      SETTINGS_RX.TIME = MicroClock();
      SETTINGS_RX.RXFLAG = DoCAN_RX_COMPLETE;
      SETTINGS_RX.STATUS = DoCAN_Idle;
    }
    else {
      CONFIG.ERRORCODE = DoCAN_ERR_FRAMEISSUE;
      SETTINGS_RX.STATUS = DoCAN_Idle;
      SETTINGS_RX.RXFLAG = DoCAN_RX_ERROR;
      return;
    }
  }
  else {
    CONFIG.ERRORCODE = DoCAN_ERR_FRAMEISSUE;
    SETTINGS_RX.STATUS = DoCAN_Idle;
    SETTINGS_RX.RXFLAG = DoCAN_RX_ERROR;
    return;
  }
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        FrameRX_FF
 * @class       ISO_DoCAN (Private)
 * @brief       Receive First Frame
 * @param []    Nothing
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_DoCAN::FrameRX_FF (void) {
  if (SETTINGS_RX.STATUS == DoCAN_Receive) {
    uint16_t LEN = 0;
    LEN = (uint16_t)( ((MESSAGE.DATA[0] & 0x0F) << 8) | MESSAGE.DATA[1] );
    if ( (LEN > 7) && (LEN <= CONFIG.LENGTH) ) {
      *CONFIG.LEN = LEN;
      *CONFIG.ID = MESSAGE.ID;
      SETTINGS_RX.LENGTH = LEN;
      LEN = LEN - 6;
      if ( (LEN % 7) == 0 ) {
        SETTINGS_RX.FRAMES = (LEN / 7);
      } else {
        SETTINGS_RX.FRAMES = (LEN / 7) + 1;
      }

      SETTINGS_RX.BLOCKCOUNTER = 0;
      SETTINGS_RX.INDEX = 6;
      FrameTX_FC (0);
      if ( CONFIG.ERRORCODE == DoCAN_ERR_WRONGCANID ) {
        SETTINGS_RX.STATUS = DoCAN_Idle;
        SETTINGS_RX.RXFLAG = DoCAN_RX_ERROR;
        return;
      }
      SETTINGS_RX.TIME = MicroClock();
    }
    else {
      FrameTX_FC (2);
      CONFIG.ERRORCODE = DoCAN_ERR_FRAMEISSUE;
      SETTINGS_RX.STATUS = DoCAN_Idle;
      SETTINGS_RX.RXFLAG = DoCAN_RX_ERROR;
      return;
    }
  }
  else {
    CONFIG.ERRORCODE = DoCAN_ERR_FRAMEISSUE;
    SETTINGS_RX.STATUS = DoCAN_Idle;
    SETTINGS_RX.RXFLAG = DoCAN_RX_ERROR;
    return;
  }
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        FrameRX_CF
 * @class       ISO_DoCAN (Private)
 * @brief       Receive Consecutive Frame
 * @param []    Nothing
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_DoCAN::FrameRX_CF (void) {
  if (SETTINGS_RX.STATUS == DoCAN_Receive) {
    if ( (MESSAGE.DATA[0] & 0x0F) == SETTINGS_RX.BLOCKCOUNTER ) {
      SETTINGS_RX.BLOCKCOUNTER = (SETTINGS_RX.BLOCKCOUNTER + 1) % 16;
      for (uint8_t I = 1; I < 8; I++) {
        CONFIG.DATA[SETTINGS_RX.INDEX] = MESSAGE.DATA[I];
        SETTINGS_RX.INDEX++;
        if (SETTINGS_RX.INDEX == SETTINGS_RX.LENGTH) {
          break;
        }
      }
      if (SETTINGS_RX.INDEX == SETTINGS_RX.LENGTH) {
        SETTINGS_RX.RXFLAG = DoCAN_RX_COMPLETE;
        SETTINGS_RX.STATUS = DoCAN_Idle;
      }
      SETTINGS_RX.FRAMES--;
      SETTINGS_RX.COUNTER--;
      if ( SETTINGS_RX.COUNTER == 0 ) {
        FrameTX_FC (0);
        if ( CONFIG.ERRORCODE == DoCAN_ERR_WRONGCANID ) {
          SETTINGS_RX.STATUS = DoCAN_Idle;
          SETTINGS_RX.RXFLAG = DoCAN_RX_ERROR;
          return;
        }
      }
    }
    else {
      CONFIG.ERRORCODE = DoCAN_ERR_WRONGFRAMECOUNTER;
      SETTINGS_RX.STATUS = DoCAN_Idle;
      SETTINGS_RX.RXFLAG = DoCAN_RX_ERROR;
      return;
    }
  }
  else {
    CONFIG.ERRORCODE = DoCAN_ERR_FRAMEISSUE;
    SETTINGS_RX.STATUS = DoCAN_Idle;
    SETTINGS_RX.RXFLAG = DoCAN_RX_ERROR;
    return;
  }
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        FrameTX_FC
 * @class       ISO_DoCAN (Private)
 * @brief       Transmit Flow Control Frame
 * @param [CanID]     CAN ID
 * @param [FS]        Flow Status
 * @param [BS]        Block Size
 * @param [STMin]     ST Min Time
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_DoCAN::FrameTX_FC (uint8_t FS) {
  uint32_t CanID;
  if ( SETTINGS_RX.MODE == DoCAN_PHY ) {
    CanID = CONFIG.CANID_TX;
  }
  else if  ( SETTINGS_RX.MODE == DoCAN_FUN ) {
    CanID = CONFIG.CANID_FN;
  }
  else {
    CONFIG.ERRORCODE = DoCAN_ERR_WRONGCANID;
    SETTINGS_RX.STATUS = DoCAN_Idle;
    return;
  }
  if (FS == 0) {
    uint8_t BS = 0;
    if ( CONFIG.BLOCKS == 0 ) {
      SETTINGS_RX.COUNTER = SETTINGS_RX.FRAMES;
      BS = 0;
    }
    else {
      if ( SETTINGS_RX.FRAMES <= CONFIG.BLOCKS ) {
        SETTINGS_RX.COUNTER = SETTINGS_RX.FRAMES;
        BS = 0;
      }
      else {
        SETTINGS_RX.COUNTER = CONFIG.BLOCKS;
        BS = CONFIG.BLOCKS;
      }
    }

    uint8_t Data[8];
    Data[0] = 0x30 | (FS & 0x0F);
    Data[1] = BS;
    Data[2] = CONFIG.STMIN;
    Data[3] = CONFIG.PADDING;
    Data[4] = CONFIG.PADDING;
    Data[5] = CONFIG.PADDING;
    Data[6] = CONFIG.PADDING;
    Data[7] = CONFIG.PADDING;
    PCAN.Write (CanID, Data);
  }
  else {
    uint8_t Data[8];
    Data[0] = 0x32;
    Data[1] = 0x00;
    Data[2] = 0x00;
    Data[3] = CONFIG.PADDING;
    Data[4] = CONFIG.PADDING;
    Data[5] = CONFIG.PADDING;
    Data[6] = CONFIG.PADDING;
    Data[7] = CONFIG.PADDING;
    PCAN.Write (CanID, Data);
  }
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        DoCAN_RX
 * @class       ISO_DoCAN (Private)
 * @brief       Process Receive DoCAN Messages
 * @param []    Nothing
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_DoCAN::DoCAN_RX (void) {
  if ( SETTINGS_RX.RXFLAG != DoCAN_RX_WORKING ) {
    SETTINGS_RX.STATUS = DoCAN_Idle;
    return;
  }
  else {
    if ( SETTINGS_RX.STATUS == DoCAN_Idle ) {
      SETTINGS_RX.STATUS = DoCAN_Idle;
      return;
    }
    else {
      if ( MESSAGE.ID != CONFIG.CANID_RX ) {
        CONFIG.ERRORCODE = DoCAN_ERR_WRONGCANID;
        SETTINGS_RX.STATUS = DoCAN_Idle;
        return;
      }
      else {
        uint8_t PCI = 0;
        PCI = (MESSAGE.DATA[0] & 0xF0) >> 4;
        switch (PCI) {
          case 0x00 : { // Receiving SF
            FrameRX_SF();
            return;
          }
          case 0x01 : { // Receiving FF
            FrameRX_FF();
            return;
          }
          case 0x02 : { // Receiving CF
            FrameRX_CF();
            return;
          }
          case 0x03 : { // Receiving FC
            FrameRX_FC();
            return;
          }
          default : {
            CONFIG.ERRORCODE = DoCAN_ERR_FRAMEISSUE;
            SETTINGS_RX.STATUS = DoCAN_Idle;
            SETTINGS_RX.RXFLAG = DoCAN_RX_ERROR;
            return;
          }
        }
      }
    }
  }
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        Receive
 * @class       ISO_DoCAN (Public)
 * @brief       Receive DoCAN Frame
 * @param [Mode]    Mode Where 0 is for Normal Messages and 1 is for Flow Control
 * @param [Mode]    Timeout Where 0 is for P2* and 1 is for P2
 * @return      DoCAN Receive Status (Enum)
 */
/* ---------------------------------------------------------------------------------------------------- */
uint8_t ISO_DoCAN::Receive (uint8_t Mode, uint8_t Time) {
  SETTINGS_RX.TIME = MicroClock();
  SETTINGS_RX.RXFLAG = DoCAN_RX_WORKING;
  SETTINGS_RX.STATUS = ((Mode) ? DoCAN_Wait : DoCAN_Receive);
  uint64_t TIMEOUT = (Time) ? CONFIG.P2 : CONFIG.P2_STAR;

  do {
    if (PCAN.Read(PCAN.MESSAGE) == PCAN_ERROR_OK) {
      if (PCAN.MESSAGE.LEN == 8) {
        MESSAGE.TYPE = PCAN.MESSAGE.TYPE;
        MESSAGE.LEN = PCAN.MESSAGE.LEN;
        MESSAGE.ID = PCAN.MESSAGE.ID;
        for (int I = 0; I < 8; I++) {
          MESSAGE.DATA[I] = PCAN.MESSAGE.DATA[I];
        }
        DoCAN_RX();
      }
    }
  } while ((SETTINGS_RX.RXFLAG == DoCAN_RX_WORKING) && ((MicroClock() - SETTINGS_RX.TIME) <= TIMEOUT));
  return SETTINGS_RX.RXFLAG;
}
/* ==================================================================================================== */
 


















/* ==================================================================================================== */
/**
 * @name        SetCANID
 * @class       ISO_DoCAN (Public)
 * @brief       Start DoCAN and CAN Drivers
 * @param [ID_TX]     DoCAN CAN ID For TX
 * @param [ID_RX]     DoCAN CAN ID For RX
 * @param [ID_FN]     DoCAN CAN ID For FUN
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_DoCAN::SetCANID (uint32_t ID_TX, uint32_t ID_RX, uint32_t ID_FN) {
  CONFIG.CANID_TX = ID_TX;
  CONFIG.CANID_RX = ID_RX;
  CONFIG.CANID_FN = ID_FN;
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        SetBuffer
 * @class       ISO_DoCAN (Public)
 * @brief       Start DoCAN and CAN Drivers
 * @param [ID]        UDS Data CAN ID Variable Address
 * @param [Len]       UDS Data Length Variable Address
 * @param [Data]      UDS Data Buffer Start Address
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_DoCAN::SetBuffer (uint32_t * ID, uint16_t * Len, uint8_t * Data) {
  CONFIG.DATA = Data;
  CONFIG.LEN = Len;
  CONFIG.ID = ID;
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        SetTiming
 * @class       ISO_DoCAN (Public)
 * @brief       Start DoCAN and CAN Drivers
 * @param [P2]          DoCAN P2 Time
 * @param [P2_Star]     DoCAN P2* Time
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_DoCAN::SetTiming (uint64_t P2, uint64_t P2_Star) {
  CONFIG.P2_Star = P2_Star;
  CONFIG.P2 = P2;
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        SetUDSParameter
 * @class       ISO_DoCAN (Public)
 * @brief       Start DoCAN and CAN Drivers
 * @param [Padding]     DoCAN CAN Padding
 * @param [STMin]       DoCAN ST Min in Microseconds For Flow Control TX
 * @param [Block]       DoCAN Block Size For Flow Control TX
 * @param [Length]      UDS Buffer Length
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_DoCAN::SetUDSParameter (uint8_t Padding, uint32_t STMin, uint8_t Block, uint16_t Length) {
  STMin = STMin / 100;
  if (STMin == 0) {
    CONFIG.STMIN = 0x00;
  }
  else if (STMin < 10) {
    CONFIG.STMIN = 0xF0 | (0x0F & STMin);
  }
  else if (STMin <= 1270) {
    STMin = STMin / 10;
    CONFIG.STMIN = STMin & 0x7F;
  } else {
    CONFIG.STMIN = 0x00;
  }
  CONFIG.PADDING = Padding;
  CONFIG.BLOCKS = Block;
  CONFIG.LENGTH = Length;
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        Start
 * @class       ISO_DoCAN (Public)
 * @brief       Start DoCAN and CAN Drivers
 * @param []    Nothing
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_DoCAN::Start (void) {
  TPCANStatus Status;
  Status = PCAN.Initialize();
  if (PCAN_ERROR_OK != Status) {
    CONFIG.ERRORCODE = DoCAN_ERR_DRIVERFAILER;
    return;
  }
  Status = PCAN.Filter(CONFIG.CANID_RX);
  if (PCAN_ERROR_OK != Status) {
    CONFIG.ERRORCODE = DoCAN_ERR_DRIVERFAILER;
    return;
  }
  if (CONFIG.DATA == nullptr || CONFIG.LEN == nullptr || CONFIG.ID == nullptr) {
    CONFIG.ERRORCODE = DoCAN_ERR_INTERNALISSUE;
    return;
  }
  CONFIG.ERRORCODE = 0;
  SETTINGS_RX.STATUS = DoCAN_Idle;
    SETTINGS_RX.RXFLAG = 1;
    SETTINGS_TX.TXFLAG = 1;
    SETTINGS_RX.INDEX = 0;
    SETTINGS_RX.MODE = 0;
    SETTINGS_RX.BLOCKCOUNTER = 0;
    SETTINGS_RX.COUNTER = 0;
    SETTINGS_RX.FRAMES = 0;
  CONFIG.INIT = 1;
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        Clock
 * @class       ISO_DoCAN (Public)
 * @brief       Get Central Clock Time accoridng to Mode
 * @param [Mode]    Mode of Clock Resolution
 * @return      Time
 */
/* ---------------------------------------------------------------------------------------------------- */
uint64_t ISO_DoCAN::Clock (uint8_t Mode) {
  if (Mode) {
    return MilliClock();
  } else {
    return MicroClock();
  }
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        MicroClock
 * @class       ISO_DoCAN (Public)
 * @brief       Get Central Clock Time in MicroSeconds
 * @param []    Nothing
 * @return      Time in MicroSeconds
 */
/* ---------------------------------------------------------------------------------------------------- */
uint64_t ISO_DoCAN::MicroClock (void) {
  return std::chrono::duration_cast<std::chrono::microseconds>(Clock::now() - StartTime).count();
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        MilliClock
 * @class       ISO_DoCAN (Public)
 * @brief       Get Central Clock Time in MilliSecond
 * @param []    Nothing
 * @return      Time in MilliSecond
 */
/* ---------------------------------------------------------------------------------------------------- */
uint64_t ISO_DoCAN::MilliClock (void) {
  return std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - StartTime).count();
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        Delay
 * @class       ISO_DoCAN (Public)
 * @brief       Delay for Some Time accoridng to Mode
 * @param [Time]    Time of Delay
 * @param [Mode]    Mode of Clock Resolution
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_DoCAN::Delay (uint64_t Time, uint8_t Mode) {
  if (Mode) {
    MilliDelay (Time);
  } else {
    MicroDelay (Time);
  }
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        MicroDelay
 * @class       ISO_DoCAN (Public)
 * @brief       Delay for Some Time in MicroSeconds
 * @param [Time]    Time of Delay in MicroSeconds
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_DoCAN::MicroDelay (uint64_t Time) {
  uint64_t Entry = MicroClock();
  uint64_t Exit = MicroClock();
  while ((Exit - Entry) < Time) { Exit = MicroClock(); }
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        MilliDelay
 * @class       ISO_DoCAN (Public)
 * @brief       Delay for Some Time in MilliSecond
 * @param [Time]    Time of Delay in MilliSecond
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_DoCAN::MilliDelay (uint64_t Time) {
  uint64_t Entry = MilliClock();
  uint64_t Exit = MilliClock();
  while ((Exit - Entry) < Time) { Exit = MilliClock(); }
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @end       End of File DoCAN.h
 */
/* ---------------------------------------------------------------------------------------------------- */
#endif  // _DoCAN
/* ==================================================================================================== */
