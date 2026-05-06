/**
 * @file    Sutrika.cpp
 * @author  Alakshendra Singh
 * @brief   Sutrika Is A Universal UDS Tools
 * @details Sutrika is a Universal UDS Tool that Features UDSonCAN (ISO 14229-3) and Custom CAN Features
 *
 * @copyright Copyright (c) 2026
 * @note      For Reporting Any Issue Don't Contact Me. Fix Yourself!
 */

#include "Sutrika.hpp"

// #define FILESYSTEM_WORKING
// #define PCAN_VERBOSE
#define PCAN_SUPERFAST


#ifdef FILESYSTEM_WORKING
  #include <filesystem>
#else
  #include <direct.h>
#endif
#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <algorithm>

#include "windows.h"
#include "PCANBasic.h"

static_assert ( offsetof(PCAN_Message, DATA) == offsetof(TPCANMsg, DATA), "Layout Mismatch" );
static_assert ( sizeof(PCAN_Message) == sizeof(TPCANMsg), "Size Mismatch" );


/**
 * @brief Get Central Clock Time in MicroSeconds
 * @class Sutrika (Public)
 * @return (uint64_t) Time in MicroSeconds
 */
uint64_t Sutrika::MicroClock (void) {
  return std::chrono::duration_cast<std::chrono::microseconds>( std::chrono::steady_clock::now() -
      ClockSteady ).count();
}


/**
 * @brief Get Central Clock Time in MilliSeconds
 * @class Sutrika (Public)
 * @return (uint64_t) Time in MilliSeconds
 */
uint64_t Sutrika::MilliClock (void) {
  return std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::steady_clock::now() -
      ClockSteady ).count();
}


/**
 * @brief Delay for Some Time in MicroSeconds
 * @class Sutrika (Public)
 * @param Time Time of Delay in MicroSeconds
 */
void Sutrika::MicroDelay (uint64_t Time) {
  uint64_t Entry = MicroClock();
  uint64_t Exit = MicroClock();
  while ((Exit - Entry) < Time) { Exit = MicroClock(); }
}


/**
 * @brief Delay for Some Time in MilliSecond
 * @class Sutrika (Public)
 * @param Time Time of Delay in MilliSecond
 */
void Sutrika::MilliDelay (uint64_t Time) {
  uint64_t Entry = MilliClock();
  uint64_t Exit = MilliClock();
  while ((Exit - Entry) < Time) { Exit = MilliClock(); }
}


/**
 * @brief Resets The Time Handling Blocks
 * @class Sutrika (Private)
 */
void Sutrika::TimeReset (void) {
  auto Now = std::chrono::system_clock::now();
  auto NowIST = Now + std::chrono::seconds(19800);
  auto NowUS = std::chrono::time_point_cast<std::chrono::microseconds>(NowIST)
               .time_since_epoch().count();
  LogStatus.TODAY = NowUS / Day_InMicro;
  std::time_t t = std::chrono::system_clock::to_time_t(NowIST);
  std::tm tm{}; gmtime_s(&tm, &t);

    LogStatus.TIME.YY = tm.tm_year + 1900;
    LogStatus.TIME.MM = tm.tm_mon + 1;
    LogStatus.TIME.DD = tm.tm_mday;

  ClockSystem = Now;
  ClockSteady = std::chrono::steady_clock::now();
  return;
}


/**
 * @brief Time Stamp Generator
 * @class Sutrika (Private)
 * @return (std::string ) TimeStamp String
 */
std::string Sutrika::TimeStamp (void) {
  auto Delta = std::chrono::steady_clock::now() - ClockSteady;
  auto Now = ClockSystem + Delta;
  auto NowUS = std::chrono::time_point_cast<std::chrono::microseconds>(Now)
               .time_since_epoch().count();
  NowUS += IST_InMicro;

  uint64_t DayNow = NowUS / Day_InMicro;
  if (DayNow != LogStatus.TODAY) {
    TimeReset();
    return TimeStamp();
  }

  int HH = (NowUS / 3600000000) % 24;
  int MM = (NowUS / 60000000) % 60;
  int SS = (NowUS / 1000000) % 60;
  int MS = (NowUS / 1000) % 1000;
  int US = NowUS % 1000;
  int DisplayHour = (HH == 0) ? 12 : (HH > 12 ? HH - 12 : HH);
  const char* AMPM = (HH < 12) ? "AM" : "PM";

  char Stamp[64];
  char* p = Stamp;
  p += sprintf(p, "[ %04d.%02d.%02d ", LogStatus.TIME.YY, LogStatus.TIME.MM, LogStatus.TIME.DD);
  p += sprintf(p, "%02d:%02d:%02d:", DisplayHour, MM, SS);
  p += sprintf(p, "%03d:%03d %s ]", MS, US, AMPM);

  return Stamp;
}


/**
 * @brief Stops The Logger Block And Close The Log Files
 * @class Sutrika (Private)
 */
void Sutrika::LogStop (void) {
  if ((LogStatus.TYPE != LOG_TEXT) && (LogStatus.TYPE != LOG_UDS) && (LogStatus.TYPE != LOG_CAN)) {
    File.close();
    return;
  }
  if (File.is_open()) {
    File << "\n\nLogger Stopped\nSafely Closing ...\nSUTRIKA " << LogSuffix[LogStatus.TYPE] << " LOGGER CLOSED!\n";
    File.close();
  }
  LogStatus.STARTED = false;
  return;
}


/**
 * @brief Configures The State of Logger Block And Creates File if Needed
 * @class Sutrika (Public)
 * @param IsOn Is Logger Logging (True) or Not (False)
 * @param Type Logging Type (Enumeration)
 * @return (int) Active Low Flag
 * @retval Zero (0) All OK & Non-Zero Means Issue
 */
int Sutrika::LogState(bool IsOn, LOG_TYPE Type) {
  if (!IsOn) { LogStop(); return 0; }
  if ((Type != LOG_TEXT) && (Type != LOG_UDS) && (Type != LOG_CAN)) { return 1; }
  if (LogStatus.STARTED) {
    if (Type == LogStatus.TYPE) { return 0; }
    LogStop();
  }

  auto now = std::time(nullptr);
  std::tm* LocalTime = std::localtime(&now);

#ifdef FILESYSTEM_WORKING
  std::filesystem::path LogDir = std::filesystem::current_path() / "Logs";
  std::filesystem::create_directories(LogDir);
  std::ostringstream Name;
  Name << std::put_time(LocalTime, "%Y%m%d-%H%M") << "-" << LogSuffix[Type] << ".log";
  std::filesystem::path FullPath = LogDir / Name.str();
  bool IsNewFile = !std::filesystem::exists(FullPath);
  std::string LogFilePath = FullPath.string();
#else
  _mkdir("Logs");
  std::ostringstream Name;
  Name << "Logs/" << std::put_time(LocalTime, "%Y%m%d-%H%M") << "-" << LogSuffix[Type] << ".log";
  std::string LogFilePath = Name.str();
  std::ifstream Check(LogFilePath);
  bool IsNewFile = !Check.good();
  Check.close();
#endif

  File.open(LogFilePath, std::ios::app);
  if (!File.is_open()) { return 2; }
  if (IsNewFile) {
    File << "SUTRIKA " << LogSuffix[Type] << " LOGGER!\n";
    File << "START TIME : " << std::put_time(LocalTime, "%Y-%m-%d %H:%M") << "\n";
    File << "Logging Started . . .\n";
    TimeReset();
  }

  LogStatus.TYPE = Type;
  LogStatus.STARTED = true;
  return 0;
}


/**
 * @brief Added Raw Line Without Any Formating
 * @class Sutrika (Public)
 * @param Line Raw Line To Log
 * @note Caller Has to Take Care of The Formating
 */
void Sutrika::LogRaw (const std::string &Line) {
  if (!LogStatus.STARTED || !File.is_open()) { return; }
    File << Line;
  return;
}


/**
 * @brief Add Logs With Timestamps and NewLine in Begin
 * @class Sutrika (Public)
 * @param Type Logging Type
 * @param Line Line To Log
 */
void Sutrika::LogLine (LOG_TYPE Type, const std::string &Line) {
  if (!LogStatus.STARTED || LogStatus.TYPE != Type || !File.is_open()) { return; }
  auto Stamp = TimeStamp();
    File.write("\n", 1);
    File.write(Stamp.data(), Stamp.size());
    File.write(" ", 1);
    File.write(Line.data(), Line.size());
  return;
}


/**
 * @brief Copy Data For Receiving From CAN Bus via PCAN
 * @class Sutrika (Protected)
 * @param Source TPCANMsg Message Structure
 * @param Destination PCAN_Message Message Structure
 */
inline void Sutrika::MesageFromPCAN (const TPCANMsg &Source, PCAN_Message &Destination) {
#ifdef PCAN_SUPERFAST
    memcpy(&Destination, &Source, sizeof(Destination));
#else
  Destination.ID      = Source.ID;
  Destination.TYPE    = static_cast<PCAN_MessageType>(Source.MSGTYPE);
  Destination.LEN     = Source.LEN;
  memcpy(Destination.DATA, Source.DATA, 8);
#endif
}


/**
 * @brief Copy Data For Sending To CAN Bus via PCAN
 * @class Sutrika (Protected)
 * @param Source PCAN_Message Message Structure
 * @param Destination TPCANMsg Message Structure
 */
inline void Sutrika::MesageToPCAN (const PCAN_Message &Source, TPCANMsg &Destination) {
#ifdef PCAN_SUPERFAST
  memcpy(&Destination, &Source, sizeof(Destination));
#else
  Destination.ID      = Source.ID;
  Destination.MSGTYPE = static_cast<TPCANMessageType>(Source.TYPE);
  Destination.LEN     = Source.LEN;
  memcpy(Destination.DATA, Source.DATA, 8);
#endif
}


/**
 * @brief Initialize PCAN Driver
 * @class Sutrika (Public)
 * @param KBPS Speed in KBPS
 * @return (Sutrika_ERROR) Error Code
 */
Sutrika_ERROR Sutrika::PCAN_Initialize (const uint16_t KBPS) {
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
    std::cout << "\nPCAN Initialization Failed!\nConnect Device Properly\nError Code: " << Status;
#endif
    return Sutrika_ERR_DRIVERFAILURE;
  } else {
#ifdef PCAN_VERBOSE
    std::cout << "\nPCAN Initialized Successfully\nPCAN Ready For Use";
#endif
    return Sutrika_ERR_OK;
  }
}


/**
 * @brief PCAN Driver Uninitializer
 * @class Sutrika (Public)
 * @return (Sutrika_ERROR) Error Code
 */
Sutrika_ERROR Sutrika::PCAN_Uninitialize (void) {
  TPCANStatus Status;
  Status = CAN_Uninitialize (PCAN_USBBUS1);
#ifdef PCAN_VERBOSE
  std::cout << "\nPCAN Uninitialized\nSafe To Disconnect";
#endif
  return (Status == PCAN_ERROR_OK) ? Sutrika_ERR_OK : Sutrika_ERR_DRIVERFAILURE;
}


/**
 * @brief PCAN Driver Reset The TX & RX Queues
 * @class Sutrika (Public)
 * @return (Sutrika_ERROR) Error Code
 */
Sutrika_ERROR Sutrika::PCAN_Reset (void) {
  TPCANStatus Status = CAN_Reset(PCAN_USBBUS1);
  return (Status == PCAN_ERROR_OK) ? Sutrika_ERR_OK : Sutrika_ERR_DRIVERFAILURE;
}


/**
 * @brief Write Data on CAN with Raw Configuration
 * @class Sutrika (Public) (Overloaded)
 * @param CanID CAN ID
 * @param Data Data Array Pointer
 * @param Length Frame Data Length
 * @param Type Frame Type (Enumeration)
 * @return (Sutrika_ERROR) Error Code
 */
Sutrika_ERROR Sutrika::PCAN_Write (const uint32_t CanID, uint8_t* Data, const uint8_t Length, const 
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
  return (Status == PCAN_ERROR_OK) ? Sutrika_ERR_OK : Sutrika_ERR_DRIVERFAILURE;
}


/**
 * @brief Write Data on CAN via CAN Message Stucture
 * @class Sutrika (Public) (Overloaded)
 * @param MSG CAN Message Stucture
 * @return (Sutrika_ERROR) Error Code
 */
Sutrika_ERROR Sutrika::PCAN_Write (const PCAN_Message MSG) {
  TPCANStatus Status;
  TPCANMsg PMSG;
  MesageToPCAN (MSG, PMSG);
  BYTE I = 0;
  do {
    Status = CAN_Write (PCAN_USBBUS1, &PMSG);
    I++;
  } while ((Status != PCAN_ERROR_OK) && ( I < 5));
  return (Status == PCAN_ERROR_OK) ? Sutrika_ERR_OK : Sutrika_ERR_DRIVERFAILURE;
}


/**
 * @brief Read Data from CAN with Timestamps
 * @class DriverPCAN (Sutrika) (Overloaded)
 * @param MSG CAN Message Block Structure Pointer
 * @param Time Timestamp Structure Pointer
 * @return (Sutrika_ERROR) Error Code
 */
Sutrika_ERROR Sutrika::PCAN_Read (PCAN_Message &MSG, TPCANTimestamp &Time) {
  TPCANStatus Status;
  TPCANMsg PMSG;
  Status = CAN_Read(PCAN_USBBUS1, &PMSG, &Time);
  if (Status == PCAN_ERROR_OK) {
    MesageFromPCAN (PMSG, MSG);
  }
  return (Status == PCAN_ERROR_OK) ? Sutrika_ERR_OK : Sutrika_ERR_NOTRECEIVED;
}


/**
 * @brief Read Data from CAN
 * @class Sutrika (Public) (Overloaded)
 * @param MSG CAN Message Block Structure Pointer
 * @return (Sutrika_ERROR) Error Code
 */
Sutrika_ERROR Sutrika::PCAN_Read (PCAN_Message &MSG) {
  TPCANStatus Status;
  TPCANTimestamp Time;
  TPCANMsg PMSG;
  Status = CAN_Read(PCAN_USBBUS1, &PMSG, &Time);
  if (Status == PCAN_ERROR_OK) {
    MesageFromPCAN (PMSG, MSG);
  }
  return (Status == PCAN_ERROR_OK) ? Sutrika_ERR_OK : Sutrika_ERR_NOTRECEIVED;
}


/**
 * @brief Filter Read Range of Messages at Hardware Level
 * @class Sutrika (Public) (Overloaded)
 * @param CanIDL CAN ID Lower Limit
 * @param CanIDU CAN ID Upper Limit
 * @param Type CAN Frame Type (Enumeration)
 * @return (int) Active Low Flag
 * @retval Zero (0) All OK & Non-Zero Means Issue
 */
Sutrika_ERROR Sutrika::PCAN_Filter (const uint32_t CanIDL, const uint32_t CanIDU, const PCAN_MessageType 
    Type) {
  TPCANStatus Status;
  Status = CAN_FilterMessages(PCAN_USBBUS1, CanIDL, CanIDU, Type);
  if (PCAN_ERROR_OK != Status) {
    return Sutrika_ERR_DRIVERFAILURE;
  }
  Status = CAN_Reset(PCAN_USBBUS1);
  return (Status == PCAN_ERROR_OK) ? Sutrika_ERR_OK : Sutrika_ERR_DRIVERFAILURE;
}


/**
 * @brief Filter Read Single Messages at Hardware Level
 * @class Sutrika (Public) (Overloaded)
 * @param CanID CAN ID
 * @param Type CAN Frame Type (Enumeration)
 * @return (int) Active Low Flag
 * @retval Zero (0) All OK & Non-Zero Means Issue
 */
Sutrika_ERROR Sutrika::PCAN_Filter (const uint32_t CanID, const PCAN_MessageType Type) {
  TPCANStatus Status;
  Status = CAN_FilterMessages(PCAN_USBBUS1, CanID, CanID, Type);
  if (PCAN_ERROR_OK != Status) {
    return Sutrika_ERR_DRIVERFAILURE;
  }
  Status = CAN_Reset(PCAN_USBBUS1);
  return (Status == PCAN_ERROR_OK) ? Sutrika_ERR_OK : Sutrika_ERR_DRIVERFAILURE;
}


/**
 * @brief Show CAN Message with Timestamp
 * @class Sutrika (Public) (Overloaded)
 * @param MSG CAN Message Structure
 * @param Time Timestamp Structure
 */
void Sutrika::PCAN_Show (const PCAN_Message MSG, const TPCANTimestamp Time) {
#ifdef PCAN_VERBOSE
  uint64_t uSec = 0;
  uSec = Time.micros + (1000ULL * Time.millis) + (0x100000000ULL * 1000ULL * Time.millis_overflow);
  std::cout << std::hex << std::uppercase << std::setw(16) << std::setfill('0') << "\nTime (us) : " << uSec;
  std::cout << std::hex << std::uppercase << std::setw(3) << std::setfill('0') << " CAN ID : " << MSG.ID;
  std::cout << (( MSG.TYPE == PCAN_MESSAGE_STANDARD ) ? " : STD" : " : EXT");
  std::cout << std::hex << std::uppercase << std::setw(1) << std::setfill('0') << " : " << MSG.LEN;
  for (int I = 0; I < MSG.LEN; I++) {
    std::cout << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << " " << (int)MSG.DATA[I];
  }
#endif
}


/**
 * @brief Show CAN Message
 * @class Sutrika (Public) (Overloaded)
 * @param MSG CAN Message Structure
 */
void Sutrika::PCAN_Show (const PCAN_Message MSG) {
#ifdef PCAN_VERBOSE
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
#endif
}


/**
 * @brief Log Frame Into Logger File
 * @class Sutrika (Private)
 * @param CANID CAN ID
 * @param Type Frame Type (Enumeration)
 * @param Mode Send / Receive Mode of The Frame
 */
void Sutrika::DoCAN_LogFrame (uint32_t CANID, PCAN_MessageType Type, uint8_t* Data, char Mode) {
  static constexpr char HEX[] = "0123456789ABCDEF";
  char Message[64];
  char* P = Message;
  int Bracket = 0;
  switch ((Data[0] >> 4) & 0xF) {
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
    uint8_t Value = static_cast<uint8_t>(Data[I]);
    *P++ = HEX[Value >> 4];
    *P++ = HEX[Value & 0xF];
    if (I == Bracket) { *P++ = ']'; }
    *P++ = ' ';
  }
  *P = '\0';
  LogLine (LOG_UDS, Message);
}


/**
 * @brief Send Internal DoCAN Data on CAN via PCAN
 * @class Sutrika (Private)
 * @param IsFunctional Is Functional
 * @return (int) Active Low Flag
 * @retval Zero (0) All OK & Non-Zero Means Issue
 */
int Sutrika::DoCAN_SendData (bool IsFunctional) {
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
  DoCAN_LogFrame (MSG.ID, MSG.TYPE, MSG.DATA, 'T');
  return PCAN_Write (MSG);
}


/**
 * @brief Transmit Flow Control Frame
 * @class Sutrika (Private)
 * @param Abort Abort Receive
 * @return (int) Active Low Flag
 * @retval Zero (0) All OK & Non-Zero Means Issue
 */
int Sutrika::DoCAN_TransmitFlowControl (const bool Abort) {
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
  DoCAN_LogFrame (MSG.ID, MSG.TYPE, MSG.DATA, 'T');
  return PCAN_Write (MSG);
}


/**
 * @brief Receive Flow Control Frame
 * @class Sutrika (Private)
 * @param BS Block Size
 * @param STMin Time Between Frames
 * @return (int) Active Low Flag
 * @retval Zero (0) All OK & Non-Zero Means Issue
 */
int Sutrika::DoCAN_ReceiveFlowControl (uint8_t &BS, uint8_t &STMin) {
  uint64_t OverallTimeout = CONFIG.SETTINGS.TIMEOUT << 4;
  uint64_t OverallTime = MicroClock();
  uint64_t CycleTime = MicroClock();
  PCAN_Message MSG;
  do {
    if (PCAN_Read (MSG) == PCAN_ERROR_OK) {
      if ((MSG.TYPE == CONFIG.CANID.RX.TYPE) && (MSG.ID == CONFIG.CANID.RX.ID) && (MSG.LEN == 8)) {
        DoCAN_LogFrame (MSG.ID, MSG.TYPE, MSG.DATA, 'R');
        if (MSG.DATA[0] == 0x30) {
          BS = MSG.DATA[1];
          STMin = MSG.DATA[2];
          return 0;
        } else if (MSG.DATA[0] == 0x31) { // FS : Wait
          CycleTime = MicroClock();
        }
        else if (MSG.DATA[0] == 0x32) { // FS : Abort
          return 1;
        }
      }
    }
  } while (((MicroClock() - CycleTime) < CONFIG.SETTINGS.TIMEOUT) &&
      ((MicroClock() - OverallTime) < OverallTimeout));
  return 1;
}


/**
 * @brief Receive Single Frame According to ISO 14229-3
 * @class Sutrika (Private)
 * @return (int) Active Low Flag
 * @retval Zero (0) All OK & Non-Zero Means Issue
 */
int Sutrika::DoCAN_FrameRX_SF (void) {
  int Length = CONFIG.MESSAGE[0] & 0x0F;
  if ((Length < 8) && (Length != 0)) {
    CONFIG.RX.DONE = 0;
    CONFIG.BUFFER.LENGTH = Length;
    for (int I = 0; I < Length; I++) {
      CONFIG.BUFFER.DATA[I] = CONFIG.MESSAGE[I+1];
    }
    CONFIG.RX.DONE = 1;
    return 0;
  } else {
    CONFIG.ERRORCODE = Sutrika_ERR_FRAMEISSUE;
    CONFIG.RX.DONE = 0;
    return 1;
  }
}


/**
 * @brief Receive First Frame According to ISO 14229-3
 * @class Sutrika (Private)
 * @return (int) Active Low Flag
 * @retval Zero (0) All OK & Non-Zero Means Issue
 */
int Sutrika::DoCAN_FrameRX_FF (void) {
  const int Length = (uint16_t)( ((CONFIG.MESSAGE[0] & 0x0F) << 8) | CONFIG.MESSAGE[1] );
  if ((Length <= DoCAN_MaxLength) && (Length > 7)) {
    CONFIG.RX.LENGTH = Length;
    CONFIG.RX.FRAMES = 0;
    CONFIG.RX.INDEX = 0;
    CONFIG.RX.DONE = 0;

    CONFIG.BUFFER.LENGTH = Length;
    for (int I = 2; I < 8; I++) {
      CONFIG.BUFFER.DATA[CONFIG.RX.INDEX] = CONFIG.MESSAGE[I];
      CONFIG.RX.INDEX++;
    }

    if ( DoCAN_TransmitFlowControl (false) ) {
      CONFIG.ERRORCODE = Sutrika_ERR_INTERNALISSUE;
      CONFIG.RX.DONE = 0;
      return 1;
    }
    CONFIG.RX.TIME = MicroClock();
    CONFIG.RX.FRAMES++;
    return 0;
  } else {
    CONFIG.ERRORCODE = Sutrika_ERR_FRAMEISSUE;
    CONFIG.RX.DONE = 0;
    if ( DoCAN_TransmitFlowControl (true) ) {
      CONFIG.ERRORCODE = Sutrika_ERR_INTERNALISSUE;
    }
    return 1;
  }
}


/**
 * @brief Receive Consecutive Frame According to ISO 14229-3
 * @class Sutrika (Private)
 * @return (int) Active Low Flag
 * @retval Zero (0) All OK & Non-Zero Means Issue
 */
int Sutrika::DoCAN_FrameRX_CF (void) {
  if ( (CONFIG.MESSAGE[0] & 0x0F) == CONFIG.RX.FRAMES ) {
    CONFIG.RX.FRAMES = (CONFIG.RX.FRAMES + 1) % 16;
    CONFIG.RX.TIME = MicroClock();
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
    CONFIG.ERRORCODE = Sutrika_ERR_WRONGFRAMECOUNTER;
    CONFIG.RX.DONE = 0;
    return 1;
  }
}


/**
 * @brief Transmit Single Frame According to ISO 14229-3
 * @class Sutrika (Private)
 * @param IsFunctional Is Functional
 * @return (int) Active Low Flag
 * @retval Zero (0) All OK & Non-Zero Means Issue
 */
int Sutrika::DoCAN_FrameTX_SF (void) {
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
  return DoCAN_SendData (CONFIG.TX.ISFUNCTIONAL);
}


/**
 * @brief Transmit First Frame According to ISO 14229-3
 * @class Sutrika (Private)
 * @param IsFunctional Is Functional
 * @return (int) Active Low Flag
 * @retval Zero (0) All OK & Non-Zero Means Issue
 */
int Sutrika::DoCAN_FrameTX_FF (void) {
  CONFIG.MESSAGE[0] = ((CONFIG.TX.LENGTH >> 8) & 0x0F) | 0x10;
  CONFIG.MESSAGE[1] = (uint8_t)(CONFIG.TX.LENGTH & 0x00FF);
  int I = 2;
  while (I < 8) {
    CONFIG.MESSAGE[I] = CONFIG.BUFFER.DATA[CONFIG.TX.INDEX];
    CONFIG.TX.INDEX++;
    I++;
  }
  CONFIG.TX.DONE = 0;
  return DoCAN_SendData (CONFIG.TX.ISFUNCTIONAL);
}


/**
 * @brief Transmit Consecutive Frame According to ISO 14229-3
 * @class Sutrika (Private)
 * @param IsFunctional Is Functional
 * @return (int) Active Low Flag
 * @retval Zero (0) All OK & Non-Zero Means Issue
 */
int Sutrika::DoCAN_FrameTX_CF (void) {
  CONFIG.MESSAGE[0] = 0x20 | CONFIG.TX.FRAMES;
  CONFIG.TX.FRAMES = (CONFIG.TX.FRAMES + 1) % 16;
  int I = 1;
  while ((CONFIG.TX.INDEX < CONFIG.TX.LENGTH) && (I < 8)) {
    CONFIG.MESSAGE[I] = CONFIG.BUFFER.DATA[CONFIG.TX.INDEX];
    CONFIG.TX.INDEX++;
    I++;
  }
  while (I < 8) {
    CONFIG.MESSAGE[I] = CONFIG.SETTINGS.PADDING;
    I++;
  }
  CONFIG.TX.DONE = (CONFIG.TX.INDEX >= CONFIG.TX.LENGTH) ? 1 : 0;
  return DoCAN_SendData (CONFIG.TX.ISFUNCTIONAL);
}


/**
 * @brief Configure Settings for Transport Layer Dynamically
 * @class ISO_DoCAN (Public)
 * @param Timeout Timeout in Microseconds
 * @param Padding Padding For Transmitting Messages
 * @param STMin STMin as per ISO 14229-3
 * @return (int) Active Low Flag
 */
int Sutrika::DoCAN_SetSettings (uint64_t Timeout, uint8_t Padding, uint8_t STMin) {
  CONFIG.SETTINGS.TIMEOUT = Timeout;
  CONFIG.SETTINGS.PADDING = Padding;
  CONFIG.SETTINGS.STMIN = STMin;
  return 0;
}


/**
 * @brief Configure CAN IDs for Transport Layer Dynamically
 * @class Sutrika (Public)
 * @param TXID Transmit CAN ID
 * @param TypeTX Transmit CAN Frame Type
 * @param RXID Receive CAN ID
 * @param TypeRX Receive CAN Frame Type
 * @param FUNID Functional CAN ID
 * @param TypeFUN Functional CAN Frame Type
 * @return (int) Active Low Flag
 */
int Sutrika::DoCAN_SetCANIDs (uint32_t TXID, PCAN_MessageType TypeTX, uint32_t RXID, PCAN_MessageType TypeRX,
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
  if (PCAN_Filter (MIN, MAX, Type) != PCAN_ERROR_OK) {
    return 2;
  }
  return 0;
}


/**
 * @brief Focus Hardware Filter on Receive CAN ID Only
 * @class ISO_DoCAN (Public)
 * @return (int) Active Low Flag
 */
int Sutrika::DoCAN_FocusRX (void) {
  if ( PCAN_Filter (CONFIG.CANID.RX.ID, CONFIG.CANID.RX.TYPE) != PCAN_ERROR_OK) {
    return 1;
  } return 0;
}


/**
 * @brief Get DoCAN Error Code Name
 * @class Sutrika (Public)
 * @return (std::string) Error Code
 */
std::string Sutrika::DoCAN_GetErrorName (void) {
  switch (CONFIG.ERRORCODE) {
    case Sutrika_ERR_OK                 : { return "ALL OK"; }
    case Sutrika_ERR_INTERNALISSUE      : { return "INTERNAL ISSUE"; }
    case Sutrika_ERR_FRAMEISSUE         : { return "FRAME ISSUE"; }
    case Sutrika_ERR_WRONGFRAMECOUNTER  : { return "WRONG FRAMECOUNTER"; }
    case Sutrika_ERR_DRIVERFAILURE      : { return "DRIVER FAILURE"; }
    case Sutrika_ERR_WRONGDATA          : { return "WRONG DATA"; }
    case Sutrika_ERR_NOTRECEIVED        : { return "NOT RECEIVED DATA"; }
    default                             : { return "UNEXPECTED FAILURE"; }
  }
}


/**
 * @brief Get DoCAN Error Code
 * @class Sutrika (Public)
 * @return (DoCAN_ERROR) Error Code
 */
Sutrika_ERROR Sutrika::DoCAN_GetErrorCode (void) {
  return CONFIG.ERRORCODE;
}


/**
 * @brief Receive DoCAN Frame According to ISO 14229-3
 * @class Sutrika (Public)
 * @param IsFunctional Is Functional
 * @return (int) Active Low Flag
 * @retval Zero (0) All OK & Non-Zero Means Issue
 */
int Sutrika::DoCAN_Receive (bool IsFunctional) {
  uint64_t OverallTimeout = CONFIG.SETTINGS.TIMEOUT << 4;
  uint64_t OverallTime = MicroClock();
  CONFIG.RX.TIME = MicroClock();
  PCAN_Message MSG;
  CONFIG.RX.ISFUNCTIONAL = IsFunctional;
  bool MultiFrame = false;

  do {
    if (PCAN_Read (MSG) == Sutrika_ERR_OK) {
      if ((MSG.TYPE == CONFIG.CANID.RX.TYPE) && (MSG.ID == CONFIG.CANID.RX.ID) && (MSG.LEN == 8)) {
        DoCAN_LogFrame (MSG.ID, MSG.TYPE, MSG.DATA, 'R');
        uint8_t PCI = (MSG.DATA[0] & 0xF0) >> 4;
        if ( (PCI != 0) && (PCI != 1) && (PCI != 2) ) { continue; }

        if ((MSG.DATA[0] == 0x03) && (MSG.DATA[1] == 0x7F) && (MSG.DATA[3] == 0x78)) {
          CONFIG.RX.TIME = MicroClock();
          continue;
        }
        
        for (int I = 0; I < 8; I++) { CONFIG.MESSAGE[I] = MSG.DATA[I]; }
        CONFIG.BUFFER.CANID = MSG.ID;
        CONFIG.RX.DONE = 0;

        int Status = 0;
        if (PCI == 0) {             // Single Frame
          if (MultiFrame) { continue; }
          Status = DoCAN_FrameRX_SF();
        }
        else if (PCI == 1) {        // First Frame
          if (MultiFrame) { continue; }
          Status = DoCAN_FrameRX_FF();
          MultiFrame = true;
        } else {                    // Consecutive Frame
          Status = DoCAN_FrameRX_CF();
        }

        if (Status != 0) {
            return 1; // Processing Error
        }
        if (CONFIG.RX.DONE == 1) {
            return 0; // Reception Completed
        }

      }
    }
  } while (((MicroClock() - CONFIG.RX.TIME) < CONFIG.SETTINGS.TIMEOUT) &&
      ((MicroClock() - OverallTime) < OverallTimeout));
  return 2; // Timed Out
}


/**
 * @brief Write DoCAN Frame According to ISO 14229-3
 * @class Sutrika (Public)
 * @param IsFunctional Is Functional
 * @return (int) Active Low Flag
 * @retval Zero (0) All OK & Non-Zero Means Issue
 * @retval 1 : Internal Issue
 * @retval 2 : Wrong Value
 */
int Sutrika::DoCAN_Write (bool IsFunctional) {
  int Length = CONFIG.BUFFER.LENGTH;
  if ((Length > DoCAN_MaxLength) || (Length == 0)) { return 2; }
  CONFIG.TX.ISFUNCTIONAL = IsFunctional;
  CONFIG.TX.LENGTH = Length;
  CONFIG.TX.FRAMES = 1;
  CONFIG.TX.INDEX = 0;
  CONFIG.TX.DONE = 0;

  if (Length < 8) { // Single Frame Send
    return DoCAN_FrameTX_SF();
  }
  else {            // Multi Frame Send
    int Status = DoCAN_FrameTX_FF();
    if (Status != 0) { return Status; }
    do {

      uint8_t BS = 0, STMin = 0;
      uint64_t TimeBetweenFrame = 0;
      Status = DoCAN_ReceiveFlowControl (BS, STMin);
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
          MicroDelay (TimeBetweenFrame);
          Status = DoCAN_FrameTX_CF();
          if (Status != 0) { return Status; }
        }
        return 0;
      }
      else {
        int I = 0;
        while ((I < BS) && (CONFIG.TX.DONE != 1)) {
          MicroDelay (TimeBetweenFrame);
          Status = DoCAN_FrameTX_CF();
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


/**
 * @brief Request And Response Transfered Via DoCAN According to ISO 14229-3
 * @class Sutrika (Public)
 * @param IsFunctional Is Functional
 * @return (int) Active Low Flag
 * @retval Zero (0) All OK & Non-Zero Means Issue
 */
int Sutrika::DoCAN_Transfer (bool IsFunctional) {
  CONFIG.ERRORCODE = Sutrika_ERR_OK;
  int Status = 0;
  PCAN_Reset();
  Status = DoCAN_Write (IsFunctional);
  if (Status != 0) {
    return Status;
  }
  Status = DoCAN_Receive (IsFunctional);
  if (Status != 0) {
    return Status;
  }
  return 0;
}


/**
 * @brief Start DoCAN Driver And Start DoCAN Layer
 * @class Sutrika (Public)
 * @param Config DoCAN Configuration Structure
 * @return (DoCAN_ERROR) Error Code
 */
Sutrika_ERROR Sutrika::DoCAN_Start (DoCAN_ConfigureStructure Setting) {
  PCAN_Uninitialize();
  if ( DoCAN_SetSettings (Setting.SETTINGS.TIMEOUT, Setting.SETTINGS.PADDING, Setting.SETTINGS.STMIN) != 0)
    { return Sutrika_ERR_WRONGDATA; }

  CONFIG.SETTINGS.SPEED = Setting.SETTINGS.SPEED;
  if ( PCAN_Initialize (CONFIG.SETTINGS.SPEED) != PCAN_ERROR_OK)
    { return Sutrika_ERR_DRIVERFAILURE; }

  int Status = DoCAN_SetCANIDs (Setting.CANID.TX.ID, Setting.CANID.TX.TYPE, Setting.CANID.RX.ID, Setting.CANID.RX.TYPE, 
      Setting.CANID.FUN.ID, Setting.CANID.FUN.TYPE);
  if (Status == 1)      { return Sutrika_ERR_WRONGDATA; }
  else if (Status == 2) { return Sutrika_ERR_DRIVERFAILURE; }

  CONFIG.ERRORCODE = Sutrika_ERR_OK;
  return Sutrika_ERR_OK;
}


/**
 * @brief Stop DoCAN Driver
 * @class Sutrika (Public)
 */
void Sutrika::DoCAN_Stop (void) {
  PCAN_Uninitialize();
  return;
}


/**
 * @brief Download Data From DoCAN Driver to External Buffer
 * @class Sutrika (Public)
 * @param CANID CAN ID Received
 * @param Data Data Buffer Pointer
 * @param Length Length of Data To Be Downloaded
 */
void Sutrika::DoCAN_DataDownload (uint32_t &CANID, uint8_t* Data, uint16_t &Length) {
  Length = CONFIG.BUFFER.LENGTH;
  CANID = CONFIG.BUFFER.CANID;
  for (int I = 0; I < CONFIG.BUFFER.LENGTH; I++) {
    Data[I] = CONFIG.BUFFER.DATA[I];
  }
  return;
}


/**
 * @brief Upload Data To DoCAN Driver to External Buffer
 * @class Sutrika (Public)
 * @param CANID CAN ID To Be Transmitted
 * @param Data Data Buffer Pointer
 * @param Length Length of Data To Be Uploaded
 */
void Sutrika::DoCAN_DataUpload (uint32_t &CANID, uint8_t* Data, uint16_t &Length) {
  CONFIG.BUFFER.LENGTH = Length;
  CONFIG.BUFFER.CANID = CANID;
  for (int I = 0; I < CONFIG.BUFFER.LENGTH; I++) {
    CONFIG.BUFFER.DATA[I] = Data[I];
  }
  return;
}


/**
 * @brief Write One Byte To The DoCAN Data Buffer
 * @class Sutrika (Public)
 * @param Index Data Index
 * @param Data Data Writen
 */
void Sutrika::DoCAN_DataByteWrite (const uint16_t Index, const uint8_t &Data) {
  if (Index < DoCAN_MaxLength) {
    CONFIG.BUFFER.DATA[Index] = Data;
  }
  return;
}

/**
 * @brief Read One Byte From The DoCAN Data Buffer
 * @class Sutrika (Public)
 * @param Index Data Index
 * @param Data Data Read
 */
void Sutrika::DoCAN_DataByteRead (const uint16_t Index, uint8_t &Data) {
  if (Index < DoCAN_MaxLength) {
    Data = CONFIG.BUFFER.DATA[Index];
  }
  Data = 0;
  return;
}