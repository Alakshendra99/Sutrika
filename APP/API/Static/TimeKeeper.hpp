/* **************************************************************************************************** */
/**
 * @file        TimeKeeper.hpp
 * @author      Alakshendra Singh
 * @brief       High Precision Timer Tool For Sutrika
 * @version     2.0
 *
 * @copyright   Copyright (c) 2025
 * @note        For Reporting Any Issue Don't Contact Me. Fix Yourself!
 */
/* **************************************************************************************************** */

#ifndef TIMEKEEPER_HPP
#define TIMEKEEPER_HPP

#include <iostream>
#include <chrono>
#include <string>


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
 * @endif TIMEKEEPER_HPP
 */
#endif // TIMEKEEPER_HPP
/* **************************************************************************************************** */