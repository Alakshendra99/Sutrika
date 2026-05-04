/* **************************************************************************************************** */
/**
 * @file        TimeKeeper.cpp
 * @author      Alakshendra Singh
 * @brief       High Precision Timer Tool For Sutrika
 * @version     2.0
 *
 * @copyright   Copyright (c) 2025
 * @note        For Reporting Any Issue Don't Contact Me. Fix Yourself!
 */
/* **************************************************************************************************** */


#include "TimeKeeper.hpp"


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