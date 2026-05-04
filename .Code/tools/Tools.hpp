/* **************************************************************************************************** */
/**
 * @file        Tools.hpp
 * @author      Alakshendra Singh
 * @brief       Tools & Utities For Sutrika
 * @version     2.0
 *
 * @copyright   Copyright (c) 2025
 * @note        For Reporting Any Issue Don't Contact Me. Fix Yourself!
 */
/* **************************************************************************************************** */

#ifndef TOOLS_HPP
#define TOOLS_HPP

#include <iostream>
#include <fstream>
#include <chrono>
#include <string>


/**
 * @brief Logging Type
 * @enum
 */
typedef enum LOG_TYPE {
  LOG_TEXT = 0,
  LOG_UDS = 1,
  LOG_CAN = 2,
  LOG_MAX = 3
} LOG_TYPE;


/* **************************************************************************************************** */
/**
 * @brief Hex Digit (Nibble) To Their Respective ASCII Character
 * @class ::Global
 * @param Value Hex Value
 * @return (char) Hex Value Character
 */
inline char HexDigitToChar (unsigned Value);
/* **************************************************************************************************** */


/* **************************************************************************************************** */
/**
 * @class TimeKeeper
 * @brief Windows Time Keeping and Time Related Tools
 */
/* ---------------------------------------------------------------------------------------------------- */
class TimeKeeper {
  private : // Private Members
    std::chrono::steady_clock::time_point StartTime;

  public : // Public Members
    TimeKeeper (void) {
      StartTime = std::chrono::steady_clock::now();
    }

    uint64_t MicroClock (void);
    uint64_t MilliClock (void);
    void MicroDelay (uint64_t Time);
    void MilliDelay (uint64_t Time);
    void Reset (void);
};
/* **************************************************************************************************** */


/* **************************************************************************************************** */
/**
 * @class Logger
 * @brief Logger Block For Sutrika Loging Purposes
 */
/* ---------------------------------------------------------------------------------------------------- */
class Logger {
  private : // Private Members
    std::chrono::steady_clock::time_point ClockSteady;
    std::chrono::system_clock::time_point ClockSystem;
    static constexpr uint64_t IST = 19800ULL * 1000000ULL;
    static constexpr uint64_t Day = 86400ULL * 1000000ULL;
    uint64_t Today = 0;
    const std::string Suffix[LOG_MAX] = { "TEXT", "UDS", "CAN" };
    std::ofstream File;
    
    struct {
      LOG_TYPE TYPE = LOG_TEXT;
      bool STARTED = false;
    } STATUS;
    struct {
      int YY; int MM; int DD;
    } TIME;

    void TimeStampReset (void);
    std::string TimeStamp (void);
    void Stop (void);

  public : // Public Members
    Logger() {
      STATUS.TYPE = LOG_TEXT;
      STATUS.STARTED = false;
      Today = 0;
      TimeStampReset();
    }

    int State (bool IsOn, LOG_TYPE Type = LOG_TEXT);
    void AddRaw (const std::string &Line);
    void AddLog (LOG_TYPE Type, const std::string &Line);
};
extern Logger Log;
/* **************************************************************************************************** */


/* **************************************************************************************************** */
/**
 * @endif TOOLS_HPP
 */
#endif // TOOLS_HPP
/* **************************************************************************************************** */