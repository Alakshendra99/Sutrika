/**
 * @file    Sutrika.hpp
 * @author  Alakshendra Singh
 * @brief   Sutrika Is A Universal UDS Tools
 * @details Sutrika is a Universal UDS Tool that Features UDSonCAN (ISO 14229-3) and Custom CAN Features
 * @version 3.14
 * 
 * @copyright Copyright (c) 2026
 * @note      For Reporting Any Issue Don't Contact Me. Fix Yourself!
 */

#ifndef SUTRIKA_HPP
#define SUTRIKA_HPP

#include "windows.h"
#include "PCANBasic.h"

#include <fstream>
#include <string>
#include <chrono>
#include <stdint.h>


/**
 * @brief Logging Type
 * @enum
 */
typedef enum LOG_TYPE {
  LOG_TEXT = 0,
  LOG_UDS,
  LOG_CAN,
  LOG_MAX,
} LOG_TYPE;

/**
 * @brief PCAN CAN Message Type
 * @enum
 */
typedef enum : uint8_t {
  PCAN_STD = static_cast<uint8_t>(PCAN_MESSAGE_STANDARD),
  PCAN_EXT = static_cast<uint8_t>(PCAN_MESSAGE_EXTENDED),
} PCAN_MessageType;

/**
 * @brief DoCAN Error Codes
 * @enum
 */
typedef enum {
  Sutrika_ERR_OK = 0,
  Sutrika_ERR_INTERNALISSUE,
  Sutrika_ERR_FRAMEISSUE,
  Sutrika_ERR_WRONGFRAMECOUNTER,
  Sutrika_ERR_DRIVERFAILURE,
  Sutrika_ERR_WRONGDATA,
  Sutrika_ERR_NOTRECEIVED,
  Sutrika_ERR_MAX,
} Sutrika_ERROR;


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
    uint64_t TIMEOUT = 0;
    uint8_t PADDING = 0;
    uint8_t STMIN = 0;
    uint16_t SPEED = 500;
  } SETTINGS;
} DoCAN_ConfigureStructure;

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


/**
 * @brief 
 * @class
 */
class Sutrika 
{
  protected : // Protected Members

    static constexpr uint64_t IST_InMicro = 19800ULL * 1000000ULL;
    static constexpr uint64_t Day_InMicro = 86400ULL * 1000000ULL;
    const std::string LogSuffix[LOG_MAX] = { "TEXT", "UDS", "CAN" };
    static constexpr uint16_t DoCAN_MaxLength = 4096;

    inline char HexDigitToChar (unsigned Value) {
      return "0123456789ABCDEF"[Value & 0xF];
    }

    inline void MesageFromPCAN (const TPCANMsg &Source, PCAN_Message &Destination);
    inline void MesageToPCAN (const PCAN_Message &Source, TPCANMsg &Destination);
    

  private : // Private Members

    std::chrono::steady_clock::time_point ClockSteady;
    std::chrono::system_clock::time_point ClockSystem;
    std::ofstream File;

    struct {
      struct {
        int YY = 0; int MM = 0; int DD = 0;
      } TIME;
      LOG_TYPE TYPE = LOG_TEXT;
      bool STARTED = false;
      uint64_t TODAY = 0;
    } LogStatus;

    struct {
        Sutrika_ERROR ERRORCODE = Sutrika_ERR_OK;
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
            uint64_t TIMEOUT = 0;
            uint8_t PADDING = 0;
            uint8_t STMIN = 0;
            uint16_t SPEED = 500;
        } SETTINGS;
        struct {
            uint8_t DATA [DoCAN_MaxLength];
            uint32_t CANID = 0;
            uint16_t LENGTH = 0;
        } BUFFER;
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
    
    std::string TimeStamp (void);
    void TimeReset (void);
    void LogStop (void);

    void DoCAN_LogFrame (uint32_t CANID, PCAN_MessageType Type,  uint8_t* Data, char Mode = 'R');
    int DoCAN_SendData (bool IsFunctional = false);
    int DoCAN_ReceiveFlowControl (uint8_t &BS, uint8_t &STMin);
    int DoCAN_TransmitFlowControl (const bool Abort);
    int DoCAN_FrameRX_SF (void);
    int DoCAN_FrameRX_FF (void);
    int DoCAN_FrameRX_CF (void);
    int DoCAN_FrameTX_SF (void);
    int DoCAN_FrameTX_FF (void);
    int DoCAN_FrameTX_CF (void);


  public : // Public Members
  
    int LogState (bool IsOn, LOG_TYPE Type = LOG_TEXT);
    void LogRaw (const std::string &Line);
    void LogLine (LOG_TYPE Type, const std::string &Line);

    void MicroDelay (uint64_t Time);
    void MilliDelay (uint64_t Time);
    uint64_t MicroClock (void);
    uint64_t MilliClock (void);

    Sutrika_ERROR PCAN_Initialize (const uint16_t KBPS = 500);
    Sutrika_ERROR PCAN_Uninitialize (void);
    Sutrika_ERROR PCAN_Reset (void);
    Sutrika_ERROR PCAN_Write (const uint32_t CanID, uint8_t* Data, const uint8_t Length = 8, const PCAN_MessageType Type = PCAN_STD);
    Sutrika_ERROR PCAN_Write (const PCAN_Message MSG);
    Sutrika_ERROR PCAN_Read (PCAN_Message &MSG, TPCANTimestamp &Time);
    Sutrika_ERROR PCAN_Read (PCAN_Message &MSG);
    Sutrika_ERROR PCAN_Filter (const uint32_t CanIDL, const uint32_t CanIDU, const PCAN_MessageType Type = PCAN_STD);
    Sutrika_ERROR PCAN_Filter (const uint32_t CanID, const PCAN_MessageType Type = PCAN_STD);
    void PCAN_Show (const PCAN_Message MSG, const TPCANTimestamp Time);
    void PCAN_Show (const PCAN_Message MSG);

    int DoCAN_SetSettings (uint64_t Timeout, uint8_t Padding = 0, uint8_t STMin = 0);
    int DoCAN_SetCANIDs (uint32_t TXID, PCAN_MessageType TypeTX, uint32_t RXID, PCAN_MessageType TypeRX, 
        uint32_t FUNID, PCAN_MessageType TypeFUN);
    int DoCAN_FocusRX (void);
    Sutrika_ERROR DoCAN_GetErrorCode (void);
    std::string DoCAN_GetErrorName (void);
    
    int DoCAN_Receive (bool IsFunctional = false);
    int DoCAN_Write (bool IsFunctional = false);
    int DoCAN_Transfer (bool IsFunctional = false);
    Sutrika_ERROR DoCAN_Start (DoCAN_ConfigureStructure Config);
    void DoCAN_Stop (void);

    void DoCAN_DataDownload (uint32_t &CANID, uint8_t* Data, uint16_t &Length);
    void DoCAN_DataUpload (uint32_t &CANID, uint8_t* Data, uint16_t &Length);
    void DoCAN_DataByteWrite (const uint16_t Index, const uint8_t &Data);
    void DoCAN_DataByteRead (const uint16_t Index, uint8_t &Data);


  public : // Contructors & Destructor               

    Sutrika (void) {
      TimeReset();
      LogStatus.TYPE = LOG_TEXT;
      LogStatus.STARTED = false;
    }

    ~Sutrika (void) {
    }
};


#endif