/* ==================================================================================================== */
/*
 *  UDS.h
 *  Unified Diagnostics Services (UDS)
 *    ISO: 14229 Part 1   - Diagonostics Services
 *    ISO: 14229 Part 2   - Timing
 *    ISO: 14229 Part 3   - CAN
 *  Version: v1.3:0
 *  Developed By: Alakshendra Singh
 *  For Reporting Any Issue Don't Contact Me. Fix Yourself
*/
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @file        UDS.h
 * @author      Alakshendra Singh
 * @brief       Unified Diagnostics Services on CAN (UDS)
 * @version     1.3
 *
 * @copyright   Copyright (c) 2025
 */
/* ==================================================================================================== */

#ifndef _UDS
#define _UDS

#include "DoCAN.hpp"

/* ==================================================================================================== */
/**
 * @class       DriverPCAN
 * @brief       PCAN Driver For CAN According To ISO 11898-1
 */
/* ---------------------------------------------------------------------------------------------------- */
class ISO_UDS {
  private:
    ISO_DoCAN DoCAN;


  public:
    struct {
      uint8_t DATA[4095];
      uint16_t LEN;
      uint32_t ID;
    } MESSAGE;

    ISO_UDS (void) {
      cout << "\nUDS Driver Loaded";
    }
    ~ISO_UDS (void) {
      cout << "\nUDS Driver Unloaded";
    }

    void Start (void);

    uint64_t Clock (uint8_t Mode = 0);
    uint64_t MicroClock (void);
    uint64_t MilliClock (void);
    void Delay (uint64_t Time, uint8_t Mode = 0);
};
//ISO_UDS UDS;
/* ==================================================================================================== */











void ISO_UDS::Start (void) {
  DoCAN.SetBuffer (&MESSAGE.ID, &MESSAGE.LEN, MESSAGE.DATA);
  DoCAN.SetUDSParameter (0x00, 0x00, 0x00, 4095);
  DoCAN.SetTiming (1000000, 5000000);
  DoCAN.SetCANID (0x785, 0x78D, 0x7DF);
  DoCAN.Start();
}


















































































/* ==================================================================================================== */
/**
 * @name        Clock
 * @class       ISO_UDS (Public)
 * @brief       Get Central Clock Time accoridng to Mode
 * @param [Mode]    Mode of Clock Resolution
 * @return      Time
 */
/* ---------------------------------------------------------------------------------------------------- */
uint64_t ISO_UDS::Clock (uint8_t Mode) {
  return DoCAN.Clock (Mode);
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        MicroClock
 * @class       ISO_UDS (Public)
 * @brief       Get Central Clock Time in MicroSeconds
 * @param []    Nothing
 * @return      Time in MicroSeconds
 */
/* ---------------------------------------------------------------------------------------------------- */
uint64_t ISO_UDS::MicroClock (void) {
  return DoCAN.MicroClock();
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        MilliClock
 * @class       ISO_UDS (Public)
 * @brief       Get Central Clock Time in MilliSecond
 * @param []    Nothing
 * @return      Time in MilliSecond
 */
/* ---------------------------------------------------------------------------------------------------- */
uint64_t ISO_UDS::MilliClock (void) {
  return DoCAN.MilliClock();
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        Delay
 * @class       ISO_UDS (Public)
 * @brief       Delay for Some Time accoridng to Mode
 * @param [Time]    Time of Delay
 * @param [Mode]    Mode of Clock Resolution
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_UDS::Delay (uint64_t Time, uint8_t Mode) {
  DoCAN.Delay (Time, Mode);
}
/* ==================================================================================================== */


/* ==================================================================================================== */
/**
 * @end       End of File UDS.h
 */
/* ---------------------------------------------------------------------------------------------------- */
#endif  // _UDS
/* ==================================================================================================== */
