/* **************************************************************************************************** */
/**
 * @file        Logger.hpp
 * @author      Alakshendra Singh
 * @brief       Tools For Sutrika
 * @version     2.0
 *
 * @copyright   Copyright (c) 2025
 * @note        For Reporting Any Issue Don't Contact Me. Fix Yourself!
 */
/* **************************************************************************************************** */

#ifndef LOGGER_HPP
#define LOGGER_HPP

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
/* **************************************************************************************************** */


/* **************************************************************************************************** */
/**
 * @endif LOGGER_HPP
 */
#endif // LOGGER_HPP
/* **************************************************************************************************** */