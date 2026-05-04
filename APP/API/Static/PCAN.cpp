/* **************************************************************************************************** */
/**
 * @file        PCAN.cpp
 * @author      Alakshendra Singh
 * @brief       PCAN Driver Operation
 * @version     2.0
 *
 * @copyright   Copyright (c) 2025
 * @note        For Reporting Any Issue Don't Contact Me. Fix Yourself!
 */
/* **************************************************************************************************** */


#include "PCAN.hpp"

#include <iostream>
#include <stdio.h>
#include <cstdio>
#include <iomanip>


/* **************************************************************************************************** */
/* ---------------------------------------------------------------------------------------------------- */
/**
 * @brief Copy Data For Sending To CAN Bus via PCAN
 * @class DriverPCAN (Private)
 * @param Source PCAN_Message Message Structure
 * @param Destination TPCANMsg Message Structure
 */
inline void DriverPCAN::MesageToPCAN (const PCAN_Message &Source, TPCANMsg &Destination) {
#ifdef PCAN_SUPERFAST
  memcpy(&Destination, &Source, sizeof(Destination));
#else
  Destination.ID      = Source.ID;
  Destination.MSGTYPE = static_cast<TPCANMessageType>(Source.TYPE);
  Destination.LEN     = Source.LEN;
  memcpy(Destination.DATA, Source.DATA, 8);
#endif
}
/* **************************************************************************************************** */

/* **************************************************************************************************** */
/* ---------------------------------------------------------------------------------------------------- */
/**
 * @brief Copy Data For Receiving From CAN Bus via PCAN
 * @class DriverPCAN (Private)
 * @param Source TPCANMsg Message Structure
 * @param Destination PCAN_Message Message Structure
 */
inline void DriverPCAN::MesageFromPCAN (const TPCANMsg &Source, PCAN_Message &Destination) {
#ifdef PCAN_SUPERFAST
    memcpy(&Destination, &Source, sizeof(Destination));
#else
  Destination.ID      = Source.ID;
  Destination.TYPE    = static_cast<PCAN_MessageType>(Source.MSGTYPE);
  Destination.LEN     = Source.LEN;
  memcpy(Destination.DATA, Source.DATA, 8);
#endif
}
/* **************************************************************************************************** */

/* **************************************************************************************************** */
/* ---------------------------------------------------------------------------------------------------- */
/**
 * @brief Initialize (Overloaded)
 * @class DriverPCAN (Public)
 * @param KBPS Speed in KBPS
 * @return (TPCANStatus) PCAN Error Code
 */
TPCANStatus DriverPCAN::Initialize (const uint16_t KBPS) {
  TPCANBaudrate Speed = PCAN_BAUD_500K;
  if (KBPS == 500) {
    Speed = PCAN_BAUD_500K;
#ifdef PCAN_VERBOSE
    std::cout << "\nPCAN Speed Set To 500KBPS";
#endif
  } else if (KBPS == 250) {
    Speed = PCAN_BAUD_250K;
#ifdef PCAN_VERBOSE
    std::cout << "\nPCAN Speed Set To 250KBPS";
#endif
  } else if (KBPS == 1000) {
    Speed = PCAN_BAUD_1M;
#ifdef PCAN_VERBOSE
    std::cout << "\nPCAN Speed Set To 1MBPS";
#endif
  } else {
    Speed = PCAN_BAUD_500K;
#ifdef PCAN_VERBOSE
    std::cout << "\nPCAN Speed Defaulted To 500KBPS";
#endif
  }
  TPCANStatus Status;
  Status = CAN_Initialize (PCAN_USBBUS1, Speed);
  if (Status != PCAN_ERROR_OK) {
#ifdef PCAN_VERBOSE
    std::cout << "\nPCAN Initialization Failed!\nConnect Device Properly";
#endif
  } else {
#ifdef PCAN_VERBOSE
    std::cout << "\nPCAN Initialized Successfully\nPCAN Ready For Use";
#endif
  }
  return Status;
}
/* **************************************************************************************************** */

/* **************************************************************************************************** */
/* ---------------------------------------------------------------------------------------------------- */
/**
 * @brief PCAN Driver Uninitializer
 * @class DriverPCAN (Public)
 * @return (TPCANStatus) PCAN Error Code 
 */
TPCANStatus DriverPCAN::Uninitialize (void) {
  TPCANStatus Status;
  Status = CAN_Uninitialize (PCAN_USBBUS1);
  std::cout << "\nPCAN Uninitialized\nSafe To Disconnect";
  return Status;
}
/* **************************************************************************************************** */

/* **************************************************************************************************** */
/* ---------------------------------------------------------------------------------------------------- */
/**
 * @brief PCAN Driver Reset The TX & RX Queues
 * @class DriverPCAN (Public)
 * @return (TPCANStatus) PCAN Error Code  
 */
TPCANStatus DriverPCAN::Reset (void) {
  return CAN_Reset(PCAN_USBBUS1);
}
/* **************************************************************************************************** */

/* **************************************************************************************************** */
/* ---------------------------------------------------------------------------------------------------- */
/**
 * @brief Write Data on CAN with Raw Configuration
 * @class DriverPCAN (Public) (Overloaded)
 * @param CanID CAN ID
 * @param Data Data Array Pointer
 * @param Length Frame Data Length
 * @param Type Frame Type (Enumeration)
 * @return (uint8_t) Active Low Status Flag 
 */
uint8_t DriverPCAN::Write (const uint32_t CanID, uint8_t* Data, const uint8_t Length, const 
    PCAN_MessageType Type) {
  TPCANMsg MSG;
  MSG.MSGTYPE = Type;
  MSG.LEN = Length;
  MSG.ID = CanID;
  BYTE I;
  for (I = 0; I < MSG.LEN; I++) { MSG.DATA[I] = Data[I]; }
  TPCANStatus Status;
  I = 0;
  do {
    Status = CAN_Write (PCAN_USBBUS1, &MSG);
    I++;
  } while ((Status != PCAN_ERROR_OK) && (I < 5));
  return ((Status != PCAN_ERROR_OK) ? 1 : 0);
}
/* **************************************************************************************************** */

/* **************************************************************************************************** */
/* ---------------------------------------------------------------------------------------------------- */
/**
 * @brief Write Data on CAN via CAN Message Stucture
 * @class DriverPCAN (Public) (Overloaded)
 * @param MSG CAN Message Stucture
 * @return (uint8_t) Active Low Status Flag 
 */
uint8_t DriverPCAN::Write (const PCAN_Message MSG) {
  TPCANStatus Status;
  TPCANMsg PMSG;
  MesageToPCAN (MSG, PMSG);
  BYTE I = 0;
  do {
    Status = CAN_Write (PCAN_USBBUS1, &PMSG);
    I++;
  } while ((Status != PCAN_ERROR_OK) && ( I < 5));
  return ((Status != PCAN_ERROR_OK) ? 1 : 0);
}
/* **************************************************************************************************** */

/* **************************************************************************************************** */
/* ---------------------------------------------------------------------------------------------------- */
/**
 * @brief Read Data from CAN with Timestamps
 * @class DriverPCAN (Public) (Overloaded)
 * @param MSG CAN Message Block Structure Pointer
 * @param Time Timestamp Structure Pointer
 * @return (TPCANStatus) Status of PCAN
 */
TPCANStatus DriverPCAN::Read (PCAN_Message &MSG, TPCANTimestamp &Time) {
  TPCANStatus Status;
  TPCANMsg PMSG;
  Status = CAN_Read(PCAN_USBBUS1, &PMSG, &Time);
  MesageFromPCAN (PMSG, MSG);
  return Status;
}
/* **************************************************************************************************** */

/* **************************************************************************************************** */
/* ---------------------------------------------------------------------------------------------------- */
/**
 * @brief Read Data from CAN
 * @class DriverPCAN (Public) (Overloaded)
 * @param MSG CAN Message Block Structure Pointer
 * @return (TPCANStatus) Status of PCAN 
 */
TPCANStatus DriverPCAN::Read (PCAN_Message &MSG) {
  TPCANStatus Status;
  TPCANTimestamp Time;
  TPCANMsg PMSG;
  Status = CAN_Read(PCAN_USBBUS1, &PMSG, &Time);
  MesageFromPCAN (PMSG, MSG);
  return Status;
}
/* **************************************************************************************************** */

/* **************************************************************************************************** */
/* ---------------------------------------------------------------------------------------------------- */
/**
 * @brief Filter Read Range of Messages at Hardware Level
 * @class DriverPCAN (Public)
 * @param CanIDL CAN ID Lower Limit
 * @param CanIDU CAN ID Upper Limit
 * @param Type CAN Frame Type (Enumeration)
 * @return (TPCANStatus) Status of PCAN  
 */
TPCANStatus DriverPCAN::Filter (const uint32_t CanIDL, const uint32_t CanIDU, const PCAN_MessageType 
    Type) {
  TPCANStatus Status;
  Status = CAN_FilterMessages(PCAN_USBBUS1, CanIDL, CanIDU, Type);
  if (PCAN_ERROR_OK != Status) {
    return Status;
  }
  Status = CAN_Reset(PCAN_USBBUS1);
  return Status;
}
/* **************************************************************************************************** */

/* **************************************************************************************************** */
/* ---------------------------------------------------------------------------------------------------- */
/**
 * @brief Filter Read Single Messages at Hardware Level
 * @class DriverPCAN (Public)
 * @param CanID CAN ID
 * @param Type CAN Frame Type (Enumeration)
 * @return (TPCANStatus) Status of PCAN  
 */
TPCANStatus DriverPCAN::Filter (const uint32_t CanID, const PCAN_MessageType Type) {
  TPCANStatus Status;
  Status = CAN_FilterMessages(PCAN_USBBUS1, CanID, CanID, Type);
  if (PCAN_ERROR_OK != Status) {
    return Status;
  }
  Status = CAN_Reset(PCAN_USBBUS1);
  return Status;
}
/* **************************************************************************************************** */

/* **************************************************************************************************** */
/* ---------------------------------------------------------------------------------------------------- */
/**
 * @brief Show CAN Message with Timestamp
 * @class DriverPCAN (Public)
 * @param MSG CAN Message Structure
 * @param Time Timestamp Structure
 */
void DriverPCAN::Show (const PCAN_Message MSG, const TPCANTimestamp Time) {
  uint64_t uSec = 0;
  uSec = Time.micros + (1000ULL * Time.millis) + (0x100000000ULL * 1000ULL * Time.millis_overflow);
  std::cout << std::hex << std::uppercase << std::setw(16) << std::setfill('0') << "\nTime (us) : " << uSec;
  std::cout << std::hex << std::uppercase << std::setw(3) << std::setfill('0') << " CAN ID : " << MSG.ID;
  std::cout << (( MSG.TYPE == PCAN_MESSAGE_STANDARD ) ? " : STD" : " : EXT");
  std::cout << std::hex << std::uppercase << std::setw(1) << std::setfill('0') << " : " << MSG.LEN;
  for (int I = 0; I < MSG.LEN; I++) {
    std::cout << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << " " << (int)MSG.DATA[I];
  }
}
/* **************************************************************************************************** */

/* **************************************************************************************************** */
/* ---------------------------------------------------------------------------------------------------- */
/**
 * @brief Show CAN Message
 * @class DriverPCAN (Public)
 * @param MSG CAN Message Structure
 */
void DriverPCAN::Show (const PCAN_Message MSG) {
  std::cout << "\nCAN : ";
  std::cout << std::hex << std::uppercase << std::setw(3) << std::setfill('0') << MSG.ID;
  std::cout << std::dec;
  std::cout << (( MSG.TYPE == PCAN_MESSAGE_STANDARD ) ? " : STD : " : " : EXT : ");
  std::cout << (int)MSG.LEN;
  std::cout << " : ";
  for (int I = 0; I < MSG.LEN; I++) {
    std::cout << " ";
    std::cout << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (int)MSG.DATA[I];
  }
  std::cout << std::dec;
}
/* **************************************************************************************************** */