/* **************************************************************************************************** */
/**
 * @file        DoCAN.hpp
 * @author      Alakshendra Singh
 * @brief       Diagonstics on CAN (DoCAN)
 * @version     2.0
 *
 * @copyright   Copyright (c) 2025
 * @note        For Reporting Any Issue Don't Contact Me. Fix Yourself!
 */
/* **************************************************************************************************** */

#ifndef DoCAN_HPP
#define DoCAN_HPP


#include "DriverPCAN.hpp"
#include "Tools.hpp"


/**
 * @brief DoCAN Error Codes
 * @enum
 */
typedef enum {
  DoCAN_ERR_OK                      = 0,
  DoCAN_ERR_INTERNALISSUE           = 1,
  DoCAN_ERR_FRAMEISSUE              = 2,
  DoCAN_ERR_WRONGFRAMECOUNTER       = 3,
  DoCAN_ERR_DRIVERFAILURE           = 4,
  DoCAN_ERR_WRONGDATA               = 5,   
} DoCAN_ERROR;

/**
 * @brief DoCAN Configuration Structure
 * @struct
 */
typedef struct {
  struct {
    struct { uint32_t ID; PCAN_MessageType TYPE;
    } FUN;
    struct { uint32_t ID; PCAN_MessageType TYPE;
    } TX;
    struct { uint32_t ID; PCAN_MessageType TYPE;
    } RX;
  } CANID;
  struct {
    uint32_t* CANID = nullptr;
    uint16_t* LEN = nullptr;
    uint8_t* DATA = nullptr;
    uint16_t MAXLENGTH = 0;
  } BUFFER;
  struct {
    uint64_t TIMEOUT = 0;
    uint8_t PADDING = 0;
    uint8_t STMIN = 0;
    uint16_t SPEED = 500;
  } SETTINGS;
} DoCAN_ConfigureStructure;


/* **************************************************************************************************** */
/**
 * @class ISO_DoCAN
 * @brief Diagnostics Over CAN or UDS on CAN According To ISO 14229-3
 */
/* ---------------------------------------------------------------------------------------------------- */
class ISO_DoCAN {
  private : // Private Members 
    Logger* LOG = nullptr;  
    DriverPCAN PCAN;
    TimeKeeper Time;

    struct {
        DoCAN_ERROR ERRORCODE = DoCAN_ERR_OK;
        uint8_t MESSAGE[8];
        struct {
            struct { uint32_t ID; PCAN_MessageType TYPE;
            } FUN;
            struct { uint32_t ID; PCAN_MessageType TYPE;
            } TX;
            struct { uint32_t ID; PCAN_MessageType TYPE;
            } RX;
        } CANID;
        struct {
            uint32_t* CANID = nullptr;
            uint16_t* LEN = nullptr;
            uint8_t* DATA = nullptr;
            uint16_t MAXLENGTH = 0;
        } BUFFER;
        struct {
            uint64_t TIMEOUT = 0;
            uint8_t PADDING = 0;
            uint8_t STMIN = 0;
            uint16_t SPEED = 500;
        } SETTINGS;
        struct {
            uint64_t TIME = 0;
            uint16_t LENGTH = 0;
            uint16_t FRAMES = 0;
            uint16_t INDEX = 0;
            uint8_t DONE = 0;
            bool ISFUNCTIONAL = false;
        } RX;
        struct {
            uint16_t LENGTH = 0;
            uint16_t FRAMES = 0;
            uint16_t INDEX = 0;
            uint8_t DONE = 0;
            bool ISFUNCTIONAL = false;
        } TX;
    } CONFIG;

    void LogFrame (uint32_t CANID, PCAN_MessageType Type, char Mode = 'R');

    int SendRawData (PCAN_Message MSG);
    int SendData (bool IsFunctional = false);

    int ReceiveFlowControl (uint8_t &BS, uint8_t &STMin);
    int TransmitFlowControl (const bool Abort);

    int FrameRX_SF (void);
    int FrameRX_FF (void);
    int FrameRX_CF (void);

    int FrameTX_SF (void);
    int FrameTX_FF (void);
    int FrameTX_CF (void);
  
  public : // Public Members
    int Receive (bool IsFunctional = false);
    int Write (bool IsFunctional = false);
    int Transfer (bool IsFunctional = false);

    int SetLogger (Logger* LoggerInstance);
    int SetSettings (uint64_t Timeout, uint8_t Padding = 0, uint8_t STMin = 0);
    int SetBuffer (uint32_t* CANID, uint16_t* LEN, uint8_t* DATA, uint16_t MaxLength);
    int SetCANIDs (uint32_t TXID, PCAN_MessageType TypeTX, uint32_t RXID, PCAN_MessageType TypeRX, 
        uint32_t FUNID, PCAN_MessageType TypeFUN);
    int FocusRX (void);
    
    DoCAN_ERROR GetErrorCode (void);
    std::string GetErrorName (void);

    DoCAN_ERROR Start (DoCAN_ConfigureStructure Config);
    void Stop (void);
};
/* **************************************************************************************************** */


/* **************************************************************************************************** */
/**
 * @endif DoCAN_HPP
 */
#endif // DoCAN_HPP
/* **************************************************************************************************** */