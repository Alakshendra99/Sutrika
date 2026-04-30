/* ==================================================================================================== */
/*
*  DriverPCAN.h
*  PCAN Driver Operation
*    ISO: 11898 Part 1 - Controller Area Network (CAN)
*  Version: v1.0:0
*  Developed By: Alakshendra Singh
*  For Reporting Any Issue Don't Contact Me. Fix Yourself
*/
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @file        DriverPCAN.h
 * @author      Alakshendra Singh
 * @brief       PCAN Driver Operation
 * @version     1.0
 *
 * @copyright   Copyright (c) 2025
 */
/* ==================================================================================================== */

#ifndef _DriverPCAN
#define _DriverPCAN

#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include <cstdio>
#include <iomanip>
#include "..\PCANBasic.h"

using namespace std;

/* ==================================================================================================== */
/**
 * @class       DriverPCAN
 * @brief       PCAN Driver For CAN According To ISO 11898-1
 */
/* ---------------------------------------------------------------------------------------------------- */
class DriverPCAN {
  private:

  public:
    TPCANMsg MESSAGE;

    DriverPCAN (void) {
      cout << "\nPeak System's PCAN Driver Loaded";
    }
    ~DriverPCAN (void) {
      cout << "\nPeak System's PCAN Driver Unloaded";
    }

    TPCANStatus Initialize (uint16_t KBPS);
    TPCANStatus Initialize (void);
    TPCANStatus Uninitialize (void);

    uint8_t Write (DWORD CanID, BYTE Data[], BYTE Length = 8, TPCANMessageType Type =
        PCAN_MESSAGE_STANDARD);
    uint8_t Write (TPCANMsg MSG);

    TPCANStatus Read (TPCANMsg &MSG, TPCANTimestamp &Time);
    TPCANStatus Read (TPCANMsg &MSG);

    void Show (TPCANMsg MSG, TPCANTimestamp Time);
    void Show (TPCANMsg MSG);

    TPCANStatus Filter (DWORD CanID1, DWORD CanID2, TPCANMessageType Type);
    TPCANStatus Filter (DWORD CanID, TPCANMessageType Type = PCAN_MESSAGE_STANDARD);
};
/* ==================================================================================================== */


/* ==================================================================================================== */
/**
 * @name        Initialize (Overloaded)
 * @class       DriverPCAN (Public)
 * @brief       PCAN Driver Initializer
 * @param [KBPS]    Speed in KBPS
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
TPCANStatus DriverPCAN::Initialize (uint16_t KBPS) {
  TPCANStatus Status;
  TPCANBaudrate Speed = PCAN_BAUD_500K;
  cout << "\nDriver For Single Channel Peak System's PCAN Tool";
  if (KBPS == 500) {
    Speed = PCAN_BAUD_500K;
    cout << "\nPCAN Speed Set To 500KBPS";
  } else if (KBPS == 250) {
    Speed = PCAN_BAUD_250K;
    cout << "\nPCAN Speed Set To 250KBPS";
  } else if (KBPS == 1000) {
    Speed = PCAN_BAUD_1M;
    cout << "\nPCAN Speed Set To 1MBPS";
  } else {
    Speed = PCAN_BAUD_500K;
    cout << "\nPCAN Speed Defaulted To 500KBPS";
  }

  do {
    Status = CAN_Initialize(PCAN_USBBUS1, Speed);
    if (Status != PCAN_ERROR_OK) {
      cout << "\nPCAN Initialization Failed! \nConnect Device Properly";
      Sleep(5000);
    } else {
      cout << "\nPCAN Initialized Successfully\nPCAN Ready For Use";
    }
  } while (Status != PCAN_ERROR_OK);
  return Status;
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        Initialize (Overloaded)
 * @class       DriverPCAN (Public)
 * @brief       PCAN Driver Initializer
 * @param []    Nothing
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
TPCANStatus DriverPCAN::Initialize (void) {
  TPCANStatus Status;
  do {
    Status = CAN_Initialize(PCAN_USBBUS1, PCAN_BAUD_500K);
    if (Status != PCAN_ERROR_OK) {
      cout << "\nPCAN Initialization Failed! \nConnect Device Properly";
      Sleep(5000);
    } else {
      cout << "\nPCAN Initialized Successfully\nPCAN Ready For Use";
    }
  } while (Status != PCAN_ERROR_OK);
  return Status;
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        DriverPCAN (Overloaded)
 * @class       DriverPCAN (Public)
 * @brief       PCAN Driver Uninitializer
 * @param []    Nothing
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
TPCANStatus DriverPCAN::Uninitialize (void) {
  TPCANStatus Status;
  Status = CAN_Uninitialize(PCAN_USBBUS1);
  cout << "\nPCAN Uninitialized\nSafe To Disconnect";
  return Status;
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        Write (Overloaded)
 * @class       DriverPCAN (Public)
 * @brief       Write Data on CAN with Raw Configuration
 * @param [CanID]     CAN ID
 * @param [Data]      Data Array Pointer
 * @param [Length]    Frame Data Length
 * @param [Type]      Frame Type (enum)
 * @return      Status of PCAN
 */
/* ---------------------------------------------------------------------------------------------------- */
uint8_t DriverPCAN::Write (DWORD CanID, BYTE Data[], BYTE Length, TPCANMessageType Type) {
  TPCANMsg MSG;
  MSG.MSGTYPE = Type;
  MSG.LEN = Length;
  MSG.ID = CanID;
  for (int i = 0; i < MSG.LEN; i++) {
    MSG.DATA[i] = Data[i];
  }
  TPCANStatus Status;
  BYTE I = 0;
  do {
    Status = CAN_Write(PCAN_USBBUS1, &MSG);
    I++;
  } while ((Status != PCAN_ERROR_OK) && ( I < 5));
  return ((Status != PCAN_ERROR_OK) ? 1 : 0);
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        Write (Overloaded)
 * @class       DriverPCAN (Public)
 * @brief       Write Data on CAN via CAN Message Stucture
 * @param [MSG]   CAN Message Stucture
 * @return      Status of PCAN
 */
/* ---------------------------------------------------------------------------------------------------- */
uint8_t DriverPCAN::Write (TPCANMsg MSG) {
  TPCANStatus Status;
  BYTE I = 0;
  do {
    Status = CAN_Write(PCAN_USBBUS1, &MSG);
    I++;
  } while ((Status != PCAN_ERROR_OK) && ( I < 5));
  return ((Status != PCAN_ERROR_OK) ? 1 : 0);
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        Read (Overloaded)
 * @class       DriverPCAN (Public)
 * @brief       Read Data from CAN with Timestamps
 * @param [MSG]     CAN Message Block Structure Pointer
 * @param [Time]    Timestamp Structure Pointer
 * @return      Status of PCAN
 */
/* ---------------------------------------------------------------------------------------------------- */
TPCANStatus DriverPCAN::Read (TPCANMsg &MSG, TPCANTimestamp &Time) {
  TPCANStatus Status;
  Status = CAN_Read(PCAN_USBBUS1, &MSG, &Time);
  return Status;
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        Read (Overloaded)
 * @class       DriverPCAN (Public)
 * @brief       Read Data from CAN
 * @param [MSG]     CAN Message Block Structure Pointer
 * @return      Status of PCAN
 */
/* ---------------------------------------------------------------------------------------------------- */
TPCANStatus DriverPCAN::Read (TPCANMsg &MSG) {
  TPCANTimestamp Time;
  TPCANStatus Status;
  Status = CAN_Read(PCAN_USBBUS1, &MSG, &Time);
  return Status;
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        Show (Overloaded)
 * @class       DriverPCAN (Public)
 * @brief       Show CAN Message with Timestamp
 * @param [MSG]     CAN Message Structure
 * @param [Time]    Timestamp Structure
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void DriverPCAN::Show (TPCANMsg MSG, TPCANTimestamp Time) {
  uint64_t uSec = 0;
  uSec = Time.micros + (1000ULL * Time.millis) + (0x100000000ULL * 1000ULL * Time.millis_overflow);
  cout << std::hex << std::uppercase << std::setw(16) << std::setfill('0') << "\nTime (us) : " << uSec;
  cout << std::hex << std::uppercase << std::setw(3) << std::setfill('0') << " CAN ID : " << MSG.ID;
  cout << (( MSG.MSGTYPE == PCAN_MESSAGE_STANDARD ) ? " : STD" : " : EXT");
  cout << std::hex << std::uppercase << std::setw(1) << std::setfill('0') << " : " << MSG.LEN;
  for (int I = 0; I < MSG.LEN; I++) {
    cout << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << " " << (int)MSG.DATA[I];
  }
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        Show (Overloaded)
 * @class       DriverPCAN (Public)
 * @brief       Show CAN Message
 * @param [MSG]     CAN Message Structure
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void DriverPCAN::Show (TPCANMsg MSG) {
  cout << "\nCAN : ";
  cout << std::hex << std::uppercase << std::setw(3) << std::setfill('0') << MSG.ID;
  cout << std::dec;
  cout << (( MSG.MSGTYPE == PCAN_MESSAGE_STANDARD ) ? " : STD : " : " : EXT : ");
  cout << (int)MSG.LEN;
  cout << " : ";
  for (int I = 0; I < MSG.LEN; I++) {
    cout << " ";
    cout << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (int)MSG.DATA[I];
  }
  cout << std::dec;
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        Filter (Overloaded)
 * @class       DriverPCAN (Public)
 * @brief       Filter Read Range of Messages at Hardware Level
 * @param [CanID1]      CAN ID Lower Limit
 * @param [CanID2]      CAN ID Upper Limit
 * @param [Type]        CAN Frame Type (Enum)
 * @return      Status of PCAN
 */
/* ---------------------------------------------------------------------------------------------------- */
TPCANStatus DriverPCAN::Filter (DWORD CanID1, DWORD CanID2, TPCANMessageType Type) {
  TPCANStatus Status;
  Status = CAN_FilterMessages(PCAN_USBBUS1, CanID1, CanID2, Type);
  if (PCAN_ERROR_OK != Status) {
    return Status;
  }
  Status = CAN_Reset(PCAN_USBBUS1);
  return Status;
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        Filter (Overloaded)
 * @class       DriverPCAN (Public)
 * @brief       Filter Read Single Messages at Hardware Level
 * @param [CanID]       CAN ID
 * @param [Type]        CAN Frame Type (Enum)
 * @return      Status of PCAN
 */
/* ---------------------------------------------------------------------------------------------------- */
TPCANStatus DriverPCAN::Filter (DWORD CanID, TPCANMessageType Type) {
  TPCANStatus Status;
  Status = CAN_FilterMessages(PCAN_USBBUS1, CanID, CanID, Type);
  if (PCAN_ERROR_OK != Status) {
    return Status;
  }
  Status = CAN_Reset(PCAN_USBBUS1);
  return Status;
}
/* ==================================================================================================== */


/* ==================================================================================================== */
/**
 * @end       End of File DriverPCAN.h
 */
/* ---------------------------------------------------------------------------------------------------- */
#endif  // _DriverPCAN
/* ==================================================================================================== */