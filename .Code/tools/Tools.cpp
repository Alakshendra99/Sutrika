/* **************************************************************************************************** */
/**
 * @file        Tools.cpp
 * @author      Alakshendra Singh
 * @brief       Tools & Utities For Sutrika
 * @version     2.0
 *
 * @copyright   Copyright (c) 2025
 * @note        For Reporting Any Issue Don't Contact Me. Fix Yourself!
 */
/* **************************************************************************************************** */


#include "Tools.hpp"

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <ctime>

Logger Log;

/* **************************************************************************************************** */
/* ---------------------------------------------------------------------------------------------------- */
/**
 * @brief Get Central Clock Time in MicroSeconds
 * @class TimeKeeper (Public)
 * @return (uint64_t) Time in MicroSeconds
 */
uint64_t TimeKeeper::MicroClock (void) {
  return std::chrono::duration_cast<std::chrono::microseconds>( std::chrono::steady_clock::now() - 
      StartTime ).count();
}
/* **************************************************************************************************** */

/* **************************************************************************************************** */
/* ---------------------------------------------------------------------------------------------------- */
/**
 * @brief Get Central Clock Time in MilliSeconds
 * @class TimeKeeper (Public)
 * @return (uint64_t) Time in MilliSeconds
 */
uint64_t TimeKeeper::MilliClock (void) {
  return std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::steady_clock::now() - 
      StartTime ).count();
}
/* **************************************************************************************************** */

/* **************************************************************************************************** */
/* ---------------------------------------------------------------------------------------------------- */
/**
 * @brief Delay for Some Time in MicroSeconds
 * @class TimeKeeper (Public)
 * @param Time Time of Delay in MicroSeconds
 */
void TimeKeeper::MicroDelay (uint64_t Time) {
  uint64_t Entry = MicroClock();
  uint64_t Exit = MicroClock();
  while ((Exit - Entry) < Time) { Exit = MicroClock(); }
}
/* **************************************************************************************************** */

/* **************************************************************************************************** */
/* ---------------------------------------------------------------------------------------------------- */
/**
 * @brief Delay for Some Time in MilliSecond
 * @class TimeKeeper (Public)
 * @param Time Time of Delay in MilliSecond
 */
void TimeKeeper::MilliDelay (uint64_t Time) {
  uint64_t Entry = MilliClock();
  uint64_t Exit = MilliClock();
  while ((Exit - Entry) < Time) { Exit = MilliClock(); }
}
/* **************************************************************************************************** */

/* **************************************************************************************************** */
/* ---------------------------------------------------------------------------------------------------- */
/**
 * @brief Resets The Central Timer
 * @class TimeKeeper (Public)
 */
void TimeKeeper::Reset (void) {
  StartTime = std::chrono::steady_clock::now();
}
/* **************************************************************************************************** */

/* **************************************************************************************************** */
/* ---------------------------------------------------------------------------------------------------- */
/**
 * @brief Resets The Logger Start Time
 * @class Logger (Private)
 */
void Logger::TimeStampReset (void) {
  auto Now = std::chrono::system_clock::now();
  auto NowIST = Now + std::chrono::seconds(19800);
  auto NowUS = std::chrono::time_point_cast<std::chrono::microseconds>(NowIST)
               .time_since_epoch().count();
  Today = NowUS / Day;
  std::time_t t = std::chrono::system_clock::to_time_t(NowIST);
  std::tm tm{}; gmtime_s(&tm, &t);

  TIME.YY = tm.tm_year + 1900;
  TIME.MM = tm.tm_mon + 1;
  TIME.DD = tm.tm_mday;

  ClockSystem = Now;
  ClockSteady = std::chrono::steady_clock::now();
}
/* **************************************************************************************************** */

/* **************************************************************************************************** */
/* ---------------------------------------------------------------------------------------------------- */
/**
 * @brief Log Time Stamp Generator
 * @class Logger (Private)
 * @return (std::string ) TimeStamp String
 */
std::string Logger::TimeStamp (void) {
  auto Delta = std::chrono::steady_clock::now() - ClockSteady;
  auto Now = ClockSystem + Delta;
  auto NowUS = std::chrono::time_point_cast<std::chrono::microseconds>(Now)
               .time_since_epoch().count();
  NowUS += IST;
  
  uint64_t DayNow = NowUS / Day;
  if (DayNow != Today) {
    TimeStampReset();
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
  p += sprintf(p, "[ %04d.%02d.%02d ", TIME.YY, TIME.MM, TIME.DD);
  p += sprintf(p, "%02d:%02d:%02d:", DisplayHour, MM, SS);
  p += sprintf(p, "%03d:%03d %s ]", MS, US, AMPM);

  return Stamp;
}
/* **************************************************************************************************** */

/* **************************************************************************************************** */
/* ---------------------------------------------------------------------------------------------------- */
/**
 * @brief Stops The logger Block And Close The Log Files
 * @class Logger (Private)
 */
void Logger::Stop (void) {
  if ((STATUS.TYPE != LOG_TEXT) && (STATUS.TYPE != LOG_UDS) && (STATUS.TYPE != LOG_CAN)) {
    File.close();
    return;
  }
  if (File.is_open()) {
    File << "\n\nLogger Stopped\nSafely Closing ...\nSUTRIKA " << Suffix[STATUS.TYPE] << " LOGGER CLOSED!";
    File.close();
  }
  STATUS.STARTED = false;
  return;
}
/* **************************************************************************************************** */

/* **************************************************************************************************** */
/* ---------------------------------------------------------------------------------------------------- */
/**
 * @brief Configures The State of Logger Block And Creates File if Needed
 * @class Logger (Public)
 * @param Type Logging Type
 * @param IsOn Is Logger 
 * @return (int) Active Low Flag
 * @retval Zero (0) All OK & Non-Zero Means Issue
 */
int Logger::State (bool IsOn, LOG_TYPE Type) {
  if (!IsOn) { Stop(); return 0; }
  if ((Type != LOG_TEXT) && (Type != LOG_UDS) && (Type != LOG_CAN)) {
    return 1;
  }
  if (STATUS.STARTED) {
    if (Type == STATUS.TYPE) { return 0; }
    Stop();
  }

  std::filesystem::path exeDir = std::filesystem::current_path();
  std::filesystem::path LogDir = exeDir / "Logs";
  std::filesystem::create_directories(LogDir);
  auto now = std::chrono::system_clock::now();
  std::time_t now_c = std::chrono::system_clock::to_time_t(now);
  std::tm LocalTime{};
  localtime_s(&LocalTime, &now_c);

  std::ostringstream Name;
  Name << std::put_time(&LocalTime, "%Y%m%d-%H%M") << "-" << Suffix[Type] << ".log";
  std::filesystem::path FullPath = LogDir / Name.str();
  bool IsNewFile = !std::filesystem::exists(FullPath);
  File.open(FullPath, std::ios::app);
  if (!File.is_open()) {
    return 2;
  }

  if (IsNewFile) {
    File << "SUTRIKA " << Suffix[Type] << " LOGGER!";
    File << "\nStart Time : " << std::put_time(&LocalTime, "%Y-%m-%d %H:%M");
    File << "\nLogging Started . . .\n";
    TimeStampReset();
  }

  STATUS.TYPE = Type;
  STATUS.STARTED = true;
  return 0;
}
/* **************************************************************************************************** */

/* **************************************************************************************************** */
/* ---------------------------------------------------------------------------------------------------- */
/**
 * @brief Added Raw Line Without Any Formating
 * @class Logger (Public)
 * @param Line Line To Log
 * @note Caller Has to Take Care of The Formating
 */
void Logger::AddRaw (const std::string& Line) {
  if (!STATUS.STARTED || !File.is_open()) { return; }
    File.write(Line.data(), Line.size());
  return;
}
/* **************************************************************************************************** */

/* **************************************************************************************************** */
/* ---------------------------------------------------------------------------------------------------- */
/**
 * @brief Configures The State of Logger Block And Creates File if Needed
 * @class Logger (Public)
 * @param Type Logging Type
 * @param Line Line To Log
 */
void Logger::AddLog(LOG_TYPE Type, const std::string& Line) {
  if (!STATUS.STARTED || STATUS.TYPE != Type || !File.is_open()) { return; }
  auto Stamp = TimeStamp();
    File.write("\n", 1);
    File.write(Stamp.data(), Stamp.size());
    File.write(" ", 1);
    File.write(Line.data(), Line.size());
  return;
}
/* **************************************************************************************************** */

/* **************************************************************************************************** */
/* ---------------------------------------------------------------------------------------------------- */
/**
 * @brief Hex Digit (Nibble) To Their Respective ASCII Character
 * @class ::Global
 * @param Value Hex Value
 * @return (char) Hex Value Character
 */
inline char HexDigitToChar (unsigned Value) {
  return "0123456789ABCDEF"[Value & 0xF];
}
/* **************************************************************************************************** */