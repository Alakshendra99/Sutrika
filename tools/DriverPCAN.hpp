/* **************************************************************************************************** */
/**
 * @file        DriverPCAN.hpp
 * @author      Alakshendra Singh
 * @brief       PCAN Driver Operation
 * @version     2.0
 *
 * @copyright   Copyright (c) 2025
 * @note        For Reporting Any Issue Don't Contact Me. Fix Yourself!
 */
/* **************************************************************************************************** */

#ifndef DriverPCAN_HPP
#define DriverPCAN_HPP


#include <windows.h>
#include <stdint.h>
#include "PCANBasic.h"

// #define PCAN_VERBOSE
#define PCAN_SUPERFAST


/**
 * @brief PCAN CAN Message Type
 * @enum
 */
typedef enum : uint8_t {
  PCAN_STD = static_cast<uint8_t>(PCAN_MESSAGE_STANDARD),
  PCAN_EXT = static_cast<uint8_t>(PCAN_MESSAGE_EXTENDED),
} PCAN_MessageType;

/**
 * @brief PCAN CAN Message Type
 * @struct
 */
typedef struct {
  uint32_t          ID;
  PCAN_MessageType  TYPE;
  uint8_t           LEN;
  uint8_t           DATA[8];
} PCAN_Message;

static_assert ( offsetof(PCAN_Message, DATA) == offsetof(TPCANMsg, DATA), "Layout Mismatch" );
static_assert ( sizeof(PCAN_Message) == sizeof(TPCANMsg), "Size Mismatch" );

/* **************************************************************************************************** */
/**
 * @class       DriverPCAN
 * @brief       PCAN Driver For CAN According To ISO 11898-1
 */
/* ---------------------------------------------------------------------------------------------------- */
class DriverPCAN {
  private:
    inline void MesageToPCAN (const PCAN_Message &Source, TPCANMsg &Destination);
    inline void MesageFromPCAN (const TPCANMsg &Source, PCAN_Message &Destination);

  public:
    TPCANMsg MESSAGE;

    TPCANStatus Initialize (const uint16_t KBPS = 500);
    TPCANStatus Uninitialize (void);

    uint8_t Write (const uint32_t CanID, uint8_t* Data, const uint8_t Length = 8, const PCAN_MessageType Type = PCAN_STD);
    uint8_t Write (const PCAN_Message MSG);

    TPCANStatus Read (PCAN_Message &MSG, TPCANTimestamp &Time);
    TPCANStatus Read (PCAN_Message &MSG);

    TPCANStatus Filter (const uint32_t CanIDL, const uint32_t CanIDU, const PCAN_MessageType Type = PCAN_STD);
    TPCANStatus Filter (const uint32_t CanID, const PCAN_MessageType Type = PCAN_STD);

    void Show (const PCAN_Message MSG, const TPCANTimestamp Time);
    void Show (const PCAN_Message MSG);
};
/* **************************************************************************************************** */


/* **************************************************************************************************** */
/**
 * @endif DriverPCAN_HPP
 */
#endif // DriverPCAN_HPP
/* **************************************************************************************************** */