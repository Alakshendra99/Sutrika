/* ==================================================================================================== */
/*
 *  UDS.h
 *  Unified Diagnostics Services on CAN (UDSonCAN)
 *    ISO: 14229 Part 1   - Diagonostics Services
 *    ISO: 14229 Part 2   - Timing
 *    ISO: 14229 Part 3   - CAN
 *  Version: v1.1:0
 */
/* ==================================================================================================== */


#ifndef _UDS
#define _UDS

#ifdef _UDSonSPI
  #include "RachanaUDS.h"
  #include "SansaadhanUDS.h"
#else
  #include "CommonIncs.h"
#endif






#ifndef UDSCANID                                                                      // UDS Server CAN IDs
  #define UDSCANID
  
  #define _UDS_RxID                   0x785                                           // UDS Server RX CAN ID
  #define _UDS_TxID                   0x78D                                           // UDS Server TX CAN ID
  
  #define _UDS_Fun1_RxID              0x069                                           // UDS Server Functional Rx ID 1
  #define _UDS_Fun1_TxID              0x096                                           // UDS Server Functional Tx ID 1
  #define _UDS_Fun2_RxID              0x0A5                                           // UDS Server Functional Rx ID 2
  #define _UDS_Fun2_TxID              0x05A                                           // UDS Server Functional Tx ID 2
  #define _UDS_Fun3_RxID              0x000                                           // UDS Server Functional Rx ID 3
  #define _UDS_Fun3_TxID              0x000                                           // UDS Server Functional Tx ID 3
  #define _UDS_Fun4_RxID              0x000                                           // UDS Server Functional Rx ID 4
  #define _UDS_Fun4_TxID              0x000                                           // UDS Server Functional Tx ID 4
  #define _UDS_Fun5_RxID              0x000                                           // UDS Server Functional Rx ID 5
  #define _UDS_Fun5_TxID              0x000                                           // UDS Server Functional Tx ID 5
  #define _UDS_Fun6_RxID              0x000                                           // UDS Server Functional Rx ID 6
  #define _UDS_Fun6_TxID              0x000                                           // UDS Server Functional Tx ID 6
  #define _UDS_Fun7_RxID              0x000                                           // UDS Server Functional Rx ID 7
  #define _UDS_Fun7_TxID              0x000                                           // UDS Server Functional Tx ID 7
  #define _UDS_Fun8_RxID              0x000                                           // UDS Server Functional Rx ID 8
  #define _UDS_Fun8_TxID              0x000                                           // UDS Server Functional Tx ID 8

  #define UDS_FuncID1                 0x01                                            // UDS Allowed Functional Address 1
  #define UDS_FuncID2                 0x02                                            // UDS Allowed Functional Address 2
  #define UDS_FuncID3                 0x04                                            // UDS Allowed Functional Address 3
  #define UDS_FuncID4                 0x08                                            // UDS Allowed Functional Address 4
  #define UDS_FuncID5                 0x10                                            // UDS Allowed Functional Address 5
  #define UDS_FuncID6                 0x20                                            // UDS Allowed Functional Address 6
  #define UDS_FuncID7                 0x40                                            // UDS Allowed Functional Address 7
  #define UDS_FuncID8                 0x80                                            // UDS Allowed Functional Address 8 
#endif

#ifndef UDSParameters                                                                 // UDS Server Parameter
  #define UDSParameters
  #define UDS_ParaBufferSize          128u                                            // UDS Parameter Message Buffer Size
  #define UDS_ParaS3Timeout           5000u                                           // UDS Parameter S3 Timeout
  #define UDS_ParaP3Timeout           5000u                                           // UDS Parameter P3 Timeout
#endif

#ifndef UDSSessions                                                                   // UDS Sessions
  #define UDSSessions
  #define UDS_Default                 0x01                                            // UDS Default Session
  #define UDS_Extended                0x02                                            // UDS Extended Session
  #define UDS_Programming             0x04                                            // UDS Programming Session
  #define UDS_Safety                  0x08                                            // UDS Safety Session
  #define UDS_Engineering             0x10                                            // UDS Engineering Session
#endif

#ifndef UDSSercurity                                                                  // UDS Security Level
  #define UDSSercurity
  #define UDS_SecurityNone            0x01                                            // UDS Security None
  #define UDS_SecurityEnhanced        0x02                                            // UDS Security Enhanced
  #define UDS_SecuritySafety          0x04                                            // UDS Security Safety
  #define UDS_SecurityProgramming     0x08                                            // UDS Security Programming
  #define UDS_SecurityEOL             0x10                                            // UDS Security EOL
#endif

#ifndef UDSServerStatus                                                               // UDS Server Status
  #define UDSServerStatus
  #define UDS_ServerFree              0x00                                            // UDS Server is Free
  #define UDS_ServerBusy              0x01                                            // UDS Server is Busy
  #define UDS_ServerReceiving         0x02                                            // UDS Server is Receiving Data
  #define UDS_ServerTransmitting      0x03                                            // UDS Server is Transmitting Data
  #define UDS_ServerWaiting           0x04                                            // UDS Server is Waiting
#endif

#ifndef UDSFrameType                                                                  // UDS Frame Type
  #define UDSFrameType
  #define UDS_FrameSF                 0x01                                            // Single Frame
  #define UDS_FrameFF                 0x02                                            // First Frame
  #define UDS_FrameFC                 0x04                                            // Flow Control Frame
  #define UDS_FrameCF                 0x08                                            // Consecutive Frame
#endif

#ifndef UDSNRC                                                                        // UDS Negative Response Code
  #define UDSNRC
  #define UDS_PositiveResponse        0x40                                            // Positive Response Added in SID
  #define UDS_NRC                     0x7F                                            // Negative Reponse SID
  #define UDS_NRC_Length              0x03                                            // Negative Response Data Length
  #define UDS_NRC_GR                  0x10                                            // General Reject
  #define UDS_NRC_SNS                 0x11                                            // Service Not Supported
  #define UDS_NRC_SFNS                0x12                                            // Sub Function Not Supported
  #define UDS_NRC_IMLIF               0x13                                            // Incorrect Message Length or Invalid Format
  #define UDS_NRC_RTL                 0x14                                            // Response Too Long
  #define UDS_NRC_BRR                 0x21                                            // Busy Repeat Request
  #define UDS_NRC_CNC                 0x22                                            // Conditions Not Correct
  #define UDS_NRC_RSE                 0x24                                            // Request Sequence Error
  #define UDS_NRC_ROOR                0x31                                            // Request Out of Range
  #define UDS_NRC_SAD                 0x33                                            // Security Access Denied
  #define UDS_NRC_AR                  0x34                                            // Authentication Required
  #define UDS_NRC_IK                  0x35                                            // Invalid Key
  #define UDS_NRC_ENA                 0x36                                            // Exceed Number of Attempts
  #define UDS_NRC_RTDNE               0x37                                            // Required Time Delay Not Expired
  #define UDS_NRC_UDNA                0x70                                            // Upload Download Not Accept
  #define UDS_NRC_TDS                 0x71                                            // Transfer Data Suspended
  #define UDS_NRC_GPF                 0x72                                            // General Programming Failure
  #define UDS_NRC_WBSC                0x73                                            // Wrong Block Sequence Counter
  #define UDS_NRC_RCRRP               0x78                                            // Request Correctly Received Response Pending
  #define UDS_NRC_SFNSIAS             0x7E                                            // Sub Function Not Supported In Active Session
  #define UDS_NRC_SNSIAS              0x7F                                            // Service Not Supported In Active Session
  #define UDS_NRC_TTH                 0x86                                            // Temperature Too High
  #define UDS_NRC_TTL                 0x87                                            // Temperature Too Low
  #define UDS_NRC_VTH                 0x92                                            // Voltage Too High
  #define UDS_NRC_VTL                 0x93                                            // Voltage Too Low
#endif



// UDS Server Details
typedef struct {
  uint16_t UDS_RxID;                                                                  // UDS Server Rx CAN ID
  uint16_t UDS_TxID;                                                                  // UDS Server Tx CAN ID
  uint8_t Session;                                                                    // UDS Server Current Session
  uint8_t Security;                                                                   // UDS Server Current Security
  uint32_t SessionTime;                                                               // UDS Server Session Start Time
  uint32_t SecurityTime;                                                              // UDS Server Security Start Time
  uint8_t Status;                                                                     // UDS Server Status
} UDS_ServerDetails;
extern UDS_ServerDetails UDS_Server;

// UDS Functional Addressing
typedef struct {
  uint8_t AddressingID;                                                               // UDS Current Addressing ID
  uint8_t FunctionalIDAvailable;                                                      // UDS Number of Functional Addressing Avialble
  uint16_t PhysicalRxID;                                                              // UDS Physical Rx ID
  uint16_t PhysicalTxID;                                                              // UDS Physical Tx ID
  uint16_t FunctionalRxID[8];                                                         // UDS Functional Address Rx IDs
  uint16_t FunctionalTxID[8];                                                         // UDS Functional Address Tx IDs
} UDS_AddressingControl;
extern UDS_AddressingControl UDS_Addressing;

// UDS Server Communication Control
typedef struct {
  uint32_t TxTimeout;                                                                 // UDS Tx Disable Timeout
  uint32_t RxTimeout;                                                                 // UDS Rx Disable Timeout
  uint32_t TxTime;                                                                    // UDS Tx Disable Entry Time
  uint32_t RxTime;                                                                    // UDS Rx Disable Entry Time
  uint8_t TxState;                                                                    // UDS Tx State (Active High)
  uint8_t RxState;                                                                    // UDS Rx State (Active High)
} UDS_CommunicationController;
extern UDS_CommunicationController UDS_Communication;

// UDS Server Message Buffer
typedef struct {
    uint16_t CANID;                                                                   // UDS Server Message CAN ID
    uint16_t Length;                                                                  // UDS Server Message Length
    uint8_t Data[UDS_ParaBufferSize];                                                 // UDS Server Message Data
} UDS_ServerMessageBuffer;
extern UDS_ServerMessageBuffer UDS_Message;

// UDS ISO Time Parameters
typedef struct {
    uint16_t P2;                                                                      // UDS ISO Timer P2_Server_Max
    uint16_t P2Star;                                                                  // UDS ISO Timer P2*_Server_Max
    uint16_t P3;                                                                      // UDS ISO Timer P3_Server_Max
    uint16_t P3Star;                                                                  // UDS ISO Timer P3*_Server_Max
    uint16_t S3;                                                                      // UDS ISO Timer S3_Server_Max
    uint16_t S3Star;                                                                  // UDS ISO Timer S3*_Server_Max
} UDS_ISOTimeParatemeters;
extern UDS_ISOTimeParatemeters UDS_ISOTime;

// UDS Security Timeouts
typedef struct {
    uint32_t Enhanced;                                                                // UDS Server Security Enhanced Timeout
    uint32_t Safety;                                                                  // UDS Server Security Safety Timeout
    uint32_t Programming;                                                             // UDS Server Security Programming Timeout
    uint32_t EOL;                                                                     // UDS Server Security EOL Timeout
} UDS_ServerSecurityTimeouts;
extern UDS_ServerSecurityTimeouts UDS_SercurityTimeouts;

// UDS Session Timeout 
typedef struct {
    uint32_t Extended;                                                                // UDS Server Extended Session Timeout
    uint32_t Programming;                                                             // UDS Server Programming Session Timeout
    uint32_t Safety;                                                                  // UDS Server Safety Session Timeout
    uint32_t Engineering;                                                             // UDS Server Engineering Session Timeout
} UDS_ServerSessionTimeouts;
extern UDS_ServerSessionTimeouts UDS_SessionTimeouts;




#include "DoCAN.h"


extern void UDS_SessionTimeout (uint32_t _Time);
extern uint8_t UDS_GetSession (void);
extern uint8_t UDS_SetSession (uint8_t _Session, uint32_t _Time);
extern void UDS_SecurityTimeout (uint32_t _Time);
extern uint8_t UDS_GetSecurity (void);
extern uint8_t UDS_SetSecurity (uint8_t _Security, uint32_t _Time);

extern void UDS_VariablesStart (void);
extern void UDS_SessionTimerUpdate (void);
extern uint8_t UDS_AddressingCheck (uint16_t _CANID, uint8_t AllowedAddress);

extern uint8_t UDS_DiagonosticsSessionControl (void);
extern uint8_t UDS_ECUReset (void);
extern uint8_t UDS_TesterPresent (void);
extern uint8_t UDS_ControlDTCSetting (void);
extern uint8_t UDS_CommunicationControl (void);
extern uint8_t UDS_SecurityAccess (void);
extern uint8_t UDS_ReadDataIdentifier (void);
extern uint8_t UDS_WriteDataIdentifier (void);


extern void UDS_Application (void);
extern void UDS_InitApp (void);
extern void UDS_MainApp (void);











/* ==================================================================================================== */
/*
 *  UDS.c
 *  Unified Diagnostics Services on CAN (UDSonCAN)
 *    ISO: 14229 Part 1   - Diagonostics Services
 *    ISO: 14229 Part 2   - Timing
 *    ISO: 14229 Part 3   - CAN
 *  Version: v1.1:0
 */
/* ==================================================================================================== */

#ifdef _UDSonSPI
  #include "RachanaUDS.h"
  #include "SansaadhanUDS.h"
#else
  #include "CommonIncs.h"
  #include "UDS.h"
  #include "DoCAN.h"
#endif

//#define ____Testing
#ifdef ____Testing
  typedef int  uint8_t;
  typedef int uint16_t;
  typedef int uint32_t;
  typedef int uint64_t;
#endif



UDS_ServerDetails UDS_Server = {0};
UDS_ServerMessageBuffer UDS_Message = {0};
UDS_ISOTimeParatemeters UDS_ISOTime = {0};
UDS_ServerSecurityTimeouts UDS_SercurityTimeouts = {0};
UDS_ServerSessionTimeouts UDS_SessionTimeouts = {0};
UDS_AddressingControl UDS_Addressing = {0};
UDS_CommunicationController UDS_Communication = {0};



/* ==================================================================================================== */
/*
 *  Misc Functions
 *
 *  void UDS_SessionTimerUpdate (void)
 *  void UDS_VariablesStart (void)
 *  uint8_t UDS_AddressingCheck (uint16_t _CANID, uint8_t AllowedAddress)
 *  
 *  UDS Server Miscellineous Functions
 */
/* ---------------------------------------------------------------------------------------------------- */
void UDS_SessionTimerUpdate (void) {
    uint32_t _Time = TP_Clock();
    UDS_Server.SessionTime = _Time;
    UDS_Server.SecurityTime = _Time;
}
/* ---------------------------------------------------------------------------------------------------- */
void UDS_VariablesStart (void) {
    // Global Variable : UDS_Server
    UDS_Server.UDS_RxID = _UDS_RxID;                                                  // UDS ECU RX CAN ID
    UDS_Server.UDS_TxID = _UDS_TxID;                                                  // UDS ECU TX CAN ID
    UDS_Server.Session = UDS_Default;                                                 // ECU Default Session
    UDS_Server.Security = UDS_SecurityNone;                                           // ECU Security Level 0
    UDS_Server.SessionTime = 0u;                                                      // ECU Session Entry Time
    UDS_Server.SecurityTime = 0u;                                                     // ECU Security Entry Time
    UDS_Server.Status = UDS_ServerFree;                                               // UDS Server Status

    // Global Variable : UDS_Message
    UDS_Message.CANID = 0u;                                                           // UDS Message CANID
    UDS_Message.Length = 0u;                                                          // UDS Message Length
    for (uint16_t i = 0; i < UDS_ParaBufferSize; i++) {
      UDS_Message.Data[i] = 0u;                                                       // UDS Message Data
    }

    // Global Variable : UDS_Communication
    UDS_Communication.TxTimeout = 600000;                                             // UDS Tx Disable Timeout
    UDS_Communication.RxTimeout = 600000;                                             // UDS Rx Disable Timeout
    UDS_Communication.TxTime = 0u;                                                    // UDS Tx Disable Entry Time
    UDS_Communication.RxTime = 0u;                                                    // UDS Rx Disable Entry Time
    UDS_Communication.TxState = 1u;                                                   // UDS Tx State
    UDS_Communication.RxState = 1u;                                                   // UDS Rx State

    // Global Variable : UDS_ISOTime
    UDS_ISOTime.P2 = 50u;                                                             // UDS ISO Timer P2_Server_Max
    UDS_ISOTime.P2Star = 5000u;                                                       // UDS ISO Timer P2*_Server_Max
    UDS_ISOTime.P3 = 0u;                                                              // UDS ISO Timer P3_Server_Max
    UDS_ISOTime.P3Star = 0u;                                                          // UDS ISO Timer P3*_Server_Max
    UDS_ISOTime.S3 = 0u;                                                              // UDS ISO Timer S3_Server_Max
    UDS_ISOTime.S3Star = 0u;                                                          // UDS ISO Timer S3*_Server_Max

    // Global Variable : UDS_SercurityTimeouts
    UDS_SercurityTimeouts.Enhanced = 5000u;                                           // UDS Server Security Enhanced Timeout
    UDS_SercurityTimeouts.EOL = 5000u;                                                // UDS Server Security EOL Timeout
    UDS_SercurityTimeouts.Programming = 5000u;                                        // UDS Server Security Programming Timeout
    UDS_SercurityTimeouts.Safety = 5000u;                                             // UDS Server Security Safety Timeout

    // Global Variable : UDS_SessionTimeouts
    UDS_SessionTimeouts.Extended = 5000u;                                             // UDS Server Extented Session Timeout
    UDS_SessionTimeouts.Programming = 5000u;                                          // UDS Server Programming Session Timeout
    UDS_SessionTimeouts.Safety = 5000u;                                               // UDS Server Safety Session Timeout
    UDS_SessionTimeouts.Engineering = 5000u;                                          // UDS Server Engineering Session Timeout

    // Global Variable : UDS_Addressing
    UDS_Addressing.AddressingID = 0u;                                                 // UDS Addressing ID For Functions To Use
    UDS_Addressing.FunctionalIDAvailable 2;                                           // UDS Functional Addresses Available
    UDS_Addressing.PhysicalRxID = _UDS_RxID;                                          // UDS Physical Rx CAN ID
    UDS_Addressing.PhysicalTxID = _UDS_TxID;                                          // UDS Physical Tx CAN ID
    UDS_Addressing.FunctionalRxID[0] = _UDS_Fun1_RxID;                                // UDS Functional Rx CAN ID Group 1
    UDS_Addressing.FunctionalTxID[0] = _UDS_Fun1_TxID;                                // UDS Functional Tx CAN ID Group 1
    UDS_Addressing.FunctionalRxID[1] = _UDS_Fun2_RxID;                                // UDS Functional Rx CAN ID Group 2
    UDS_Addressing.FunctionalTxID[1] = _UDS_Fun2_TxID;                                // UDS Functional Tx CAN ID Group 2
    
    // Global Variable : UDS_Communication
    UDS_Communication.RxTimeout = 600000u;                                            // UDS Rx Disable Timeout
    UDS_Communication.TxTimeout = 600000u;                                            // UDS Tx Disable Timeout
    UDS_Communication.RxTime = 0u;                                                    // UDS Rx Entry Time
    UDS_Communication.TxTime = 0u;                                                    // UDS Tx Entry Time
    UDS_Communication.RxState = 1u;                                                   // UDS Rx Communication State (Active High)
    UDS_Communication.TxState = 1u;                                                   // UDS Tx Communication State (Active High)
}
/* ---------------------------------------------------------------------------------------------------- */
uint8_t UDS_AddressingCheck (uint16_t _CANID, uint8_t AllowedAddress) {
    if (UDS_Addressing.PhysicalRxID == _CANID) {                                      // UDS Physical Address Check
      UDS_Addressing.AddressingID = 0;                                                // UDS Addressing ID Loaded
      return 0;
    }

    uint8_t i = 0;
    while (i < UDS_Addressing.FunctionalIDAvailable) {                                // UDS Functional Address Check Loop
      if (UDS_Addressing.FunctionalRxID[i] == _CANID) {                               // UDS Functional Address Check
        uint8_t C = 1 << i;
        if ((C & AllowedAddress) == C) {                                              // UDS Functional Address Allowed
          UDS_Addressing.AddressingID = i + 1;                                        // UDS Addressing ID Loaded
          return 0;
        } else {
          return 1;
        }
      }
      i++;
    }
    return 1;
}
/* ==================================================================================================== */


/* ==================================================================================================== */
/*
 *  Session Control
 *
 *  void UDS_SessionTimeout (uint32_t _Time)
 *  uint8_t UDS_GetSession (void);
 *  uint8_t UDS_SetSession (uint8_t _Session, uint32_t _Time)
 *
 *  UDS Server Session Timeout, Get & Set Check & Implementation
 */
/* ---------------------------------------------------------------------------------------------------- */
void UDS_SessionTimeout (uint32_t _Time) {
    switch (UDS_Server.Session) {
        case UDS_Default : {                                                          // Default Session Timeout
            UDS_Server.Session = UDS_Default;                                         // Retransitioning To Default
            break;
        }
        case UDS_Extended : {                                                         // Extented Session Timeout
            if ((_Time - UDS_Server.SessionTime) > UDS_SessionTimeouts.Extended) {    // Checking For Timeout
                UDS_Server.Session = UDS_Default;                                     // Resetting To Default
                UDS_Server.Security = UDS_SecurityNone;                               // Resetting The Security
            }
            break;
        }
        case UDS_Programming : {                                                      // Programming Session Timeout
            if ((_Time - UDS_Server.SessionTime) > UDS_SessionTimeouts.Programming) { // Checking For Timeout
                UDS_Server.Session = UDS_Default;                                     // Resetting To Default
                UDS_Server.Security = UDS_SecurityNone;                               // Resetting The Security
            }
            break;
        }
        case UDS_Safety : {                                                           // Safety Session Timeout
            if ((_Time - UDS_Server.SessionTime) > UDS_SessionTimeouts.Safety) {      // Checking For Timeout
                UDS_Server.Session = UDS_Default;                                     // Resetting To Default
                UDS_Server.Security = UDS_SecurityNone;                               // Resetting The Security
            }
            break;
        }
        case UDS_Engineering : {                                                      // Engineering Session
            if ((_Time - UDS_Server.SessionTime) > UDS_SessionTimeouts.Engineering) { // Checking For Timeout
                UDS_Server.Session = UDS_Default;                                     // Resetting To Default
                UDS_Server.Security = UDS_SecurityNone;                               // Resetting The Security
            }
            break;
        }

        default : {                                                                   // Rare Case
            UDS_Server.Session = UDS_Default;                                         // Resetting To Default
            UDS_Server.Security = UDS_SecurityNone;                                   // Resetting The Security
            break;
        }
    }
}
/* ---------------------------------------------------------------------------------------------------- */
uint8_t UDS_GetSession (void) {
    return UDS_Server.Session;                                                        // Returning Session
}
/* ---------------------------------------------------------------------------------------------------- */
uint8_t UDS_SetSession (uint8_t _Session, uint32_t _Time) {
    switch (_Session) {
        case UDS_Default : {                                                          // Default Session
            UDS_Server.Session = UDS_Default;                                         // Default Session Set
            UDS_Server.Security = UDS_SecurityNone;                                   // Security Reset
            UDS_Server.SessionTime = _Time;                                           // Session Timer Reset
            UDS_Server.SecurityTime = _Time;                                          // Security Timer Reset
            break;
        }
        case UDS_Extended : {                                                         // Extended Session
            UDS_Server.Session = UDS_Extended;                                        // Extended Session Set
            UDS_Server.Security = UDS_SecurityNone;                                   // Security Reset
            UDS_Server.SessionTime = _Time;                                           // Session Timer Reset
            UDS_Server.SecurityTime = _Time;                                          // Security Timer Reset
            break;
        }
        case UDS_Programming : {                                                      // Programming Session
            UDS_Server.Session = UDS_Programming;                                     // Programming Session Set
            UDS_Server.Security = UDS_SecurityNone;                                   // Security Reset
            UDS_Server.SessionTime = _Time;                                           // Session Timer Reset
            UDS_Server.SecurityTime = _Time;                                          // Security Timer Reset
            break;
        }
        case UDS_Safety : {                                                           // Safety Session
            UDS_Server.Session = UDS_Safety;                                          // Safety Session Set
            UDS_Server.Security = UDS_SecurityNone;                                   // Security Reset
            UDS_Server.SessionTime = _Time;                                           // Session Timer Reset
            UDS_Server.SecurityTime = _Time;                                          // Security Timer Reset
            break;
        }
        case UDS_Engineering : {                                                      // Engineering Session
            UDS_Server.Session = UDS_Engineering;                                     // Engineering Session Set
            UDS_Server.Security = UDS_SecurityNone;                                   // Security Reset
            UDS_Server.SessionTime = _Time;                                           // Session Timer Reset
            UDS_Server.SecurityTime = _Time;                                          // Security Timer Reset
            break;
        }
    }
    return UDS_Server.Session;
}
/* ====================================================================================================*/


/* ==================================================================================================== */
/*
 *  Security Control
 *
 *  void UDS_SessionTimeout (uint32_t _Time)
 *  uint8_t UDS_GetSession (void);
 *  uint8_t UDSSetSession (uint8_t _Session, uint32_t _Time)
 *
 *  UDS Server Security Timeout, Get & Set Check & Implementation
 */
/* ---------------------------------------------------------------------------------------------------- */
void UDS_SecurityTimeout (uint32_t _Time) {
    switch (UDS_Server.Security) {
        case UDS_SecurityNone : {                                                     // Security None Timeout
          UDS_Server.Security = UDS_SecurityNone;                                     // Resetting Security
          break;
        }
        case UDS_SecurityEnhanced : {                                                 // Security Enhanced Timeout
          if ((_Time - UDS_Server.SecurityTime) > UDS_SercurityTimeouts.Enhanced) {   // Security Timeout Check
                UDS_Server.Security = UDS_SecurityNone;                               // Resetting Security
          }
          break;
        }
        case UDS_SecuritySafety : {                                                   // Security Safty Timeout
          if ((_Time - UDS_Server.SecurityTime) > UDS_SercurityTimeouts.Safety) {     // Security Timeout Check
                UDS_Server.Security = UDS_SecurityNone;                               // Resetting Security
          }
          break;
        }
        case UDS_SecurityProgramming : {                                              // Security Programming Timeout
          if ((_Time - UDS_Server.SecurityTime) > UDS_SercurityTimeouts.Programming) {// Security Timeout Check
                UDS_Server.Security = UDS_SecurityNone;                               // Resetting Security
          }
          break;
        }
        case UDS_SecurityEOL : {                                                      // Security EOL Timeout
          if ((_Time - UDS_Server.SecurityTime) > UDS_SercurityTimeouts.EOL) {        // Security Timeout Check
                UDS_Server.Security = UDS_SecurityNone;                               // Resetting Security
          }
          break;
        }

        default : {                                                                   // Rare Case
            UDS_Server.Security = UDS_SecurityNone;                                   // Resetting Security
            break;
        }
    }
}
/* ---------------------------------------------------------------------------------------------------- */
uint8_t UDS_GetSecurity (void) {
    return UDS_Server.Security;                                                       // Returning Security
}
/* ---------------------------------------------------------------------------------------------------- */
uint8_t UDS_SetSecurity (uint8_t _Security, uint32_t _Time) {
    switch (_Security) {
        case UDS_SecurityNone : {                                                     // Security None
            UDS_Server.Security = UDS_SecurityNone;                                   // Security None Set
            UDS_Server.SecurityTime = _Time;                                          // Timer Resetting
            break;
        }
        case UDS_SecurityEnhanced : {                                                 // Security Enhanced
            UDS_Server.Security = UDS_SecurityEnhanced;                               // Security Enhanced Set
            UDS_Server.SecurityTime = _Time;                                          // Timer Resetting
            break;
        }
        case UDS_SecuritySafety : {                                                   // Security Safety
            UDS_Server.Security = UDS_SecuritySafety;                                 // Security Safety Set
            UDS_Server.SecurityTime = _Time;                                          // Timer Resetting
            break;
        }
        case UDS_SecurityProgramming : {                                              // Security Programming
            UDS_Server.Security = UDS_SecurityProgramming;                            // Security Programming Set
            UDS_Server.SecurityTime = _Time;                                          // Timer Resetting
            break;
        }
        case UDS_SecurityEOL : {                                                      // Security EOL
            UDS_Server.Security = UDS_SecurityProgramming;                            // Security EOL Set
            UDS_Server.SecurityTime = _Time;                                          // Timer Resetting
            break;
        }
        default : {
            UDS_Server.Security = UDS_SecurityNone;
            UDS_Server.SecurityTime = _Time;
            break;
        }
    }
    return UDS_Server.Security;
}
/* ==================================================================================================== */


/* ==================================================================================================== */
/*
 *  UDS Diagonostics Session Control
 *
 *  uint8_t UDS_DiagonosticsSessionControl (void)
 *
 *  UDS Server Service 0x10 : Diagonostics Session Control
 */
/* ---------------------------------------------------------------------------------------------------- */
uint8_t UDS_DiagonosticsSessionControl (void) {
    uint8_t _SID = UDS_Message.Data[0];                                               // Extracting SID
    uint8_t _Suppress = (UDS_Message.Data[1] & 0x80) ? 1 : 0;                         // Checking is Positive Response Is Suppressed
    uint8_t _SF = UDS_Message.Data[1] & 0x7F;                                         // Extracting Sub Function
    uint32_t _Time = TP_Clock();                                                      // Loaded Current Time
    uint8_t _CurrentSession = UDS_GetSession();                                       // UDS Get Active Current Session
    
    if (_SF == 0x01) {                                                                // Session : Default
      const uint8_t SessionTransistionAllowed = UDS_Default | UDS_Extended | UDS_Programming | UDS_Safety | UDS_Engineering;
      if ((_CurrentSession & SessionTransistionAllowed) == _CurrentSession) {         // Session Transitions Check
          UDS_SetSession(UDS_Default, _Time);                                         // Default Session Started
      } else {
          TP_SendNegativeResponse(UDS_NRC_SFNS, _SID, 'P');                           // NRC : Sub Function Not Supported
          UDS_Server.Status = UDS_ServerFree;                                         // UDS Server Status Set To Free
          return 0;
      }
    }

    else if (_SF == 0x02) {                                                           // Session : Programming
      const uint8_t SessionTransistionAllowed = UDS_Extended | UDS_Programming | UDS_Engineering;
      if ((_CurrentSession & SessionTransistionAllowed) == _CurrentSession) {         // Session Transitions Check
          UDS_SetSession(UDS_Programming, _Time);                                     // Programming Session Started
      } else {
          TP_SendNegativeResponse(UDS_NRC_SFNS, _SID, 'P');                           // NRC : Sub Function Not Supported
          UDS_Server.Status = UDS_ServerFree;                                         // UDS Server Status Set To Free
          return 0;
      }
    }

    else if (_SF == 0x03) {                                                           // Session : Extended
      const uint8_t SessionTransistionAllowed = UDS_Default | UDS_Extended | UDS_Safety | UDS_Engineering;
      if ((_CurrentSession & SessionTransistionAllowed) == _CurrentSession) {         // Session Transitions Check
          UDS_SetSession(UDS_Extended, _Time);                                        // Extended Session Started
      } else {
          TP_SendNegativeResponse(UDS_NRC_SFNS, _SID, 'P');                           // NRC : Sub Function Not Supported
          UDS_Server.Status = UDS_ServerFree;                                         // UDS Server Status Set To Free
          return 0;
      }
    }

    else if (_SF == 0x04) {                                                           // Session : Safety
      const uint8_t SessionTransistionAllowed = UDS_Default | UDS_Extended | UDS_Safety | UDS_Engineering;
      if ((_CurrentSession & SessionTransistionAllowed) == _CurrentSession) {         // Session Transitions Check
          UDS_SetSession(UDS_Safety, _Time);                                          // Safety Session Started
      } else {
          TP_SendNegativeResponse(UDS_NRC_SFNS, _SID, 'P');                           // NRC : Sub Function Not Supported
          UDS_Server.Status = UDS_ServerFree;                                         // UDS Server Status Set To Free
          return 0;
      }
    }

    else if (_SF == 0x5F) {                                                           // Session : Engineering
      const uint8_t SessionTransistionAllowed = UDS_Default | UDS_Extended | UDS_Safety | UDS_Engineering;
      if ((_CurrentSession & SessionTransistionAllowed) == _CurrentSession) {         // Session Transitions Check
          UDS_SetSession(UDS_Safety, _Time);                                          // Safety Session Started
      } else {
          TP_SendNegativeResponse(UDS_NRC_SFNS, _SID, 'P');                           // NRC : Sub Function Not Supported
          UDS_Server.Status = UDS_ServerFree;                                         // UDS Server Status Set To Free
          return 0;
      }
    }

    else {                                                                            // Session : Unknown
      TP_SendNegativeResponse(UDS_NRC_SFNS, _SID, 'P');                               // NRC : Sub Function Not Supported
      UDS_Server.Status = UDS_ServerFree;                                             // UDS Server Status Set To Free
      return 0;
    }

    if (_Suppress) {                                                                  // Checking if Positive Response is Suppressed
        UDS_Server.Status = UDS_ServerFree;                                           // UDS Server Status Set To Free
        return 1;                                                                     // Returning All OK
    } else {                                                                          // Frame Building
      UDS_Message.Length = 6;                                                         // TML Suggested
      UDS_Message.Data[0] = 0x50;                                                     // Positive Response SID
      UDS_Message.Data[1] = _SF;                                                      // Echo Sub Function
      Number16Bit P2Starmax;
      Number16Bit P2max;
      P2Starmax.Raw = UDS_ISOTime.P2Star;                                             // Extracting P2*_Max
      P2max.Raw = UDS_ISOTime.P2;                                                     // Extracting P2_Max
      UDS_Message.Data[2] = P2max.Byte.B1;                                            // Loading P2_Max High
      UDS_Message.Data[3] = P2max.Byte.B0;                                            // Loading P2_Max Low
      UDS_Message.Data[4] = P2Starmax.Byte.B1;                                        // Loading P2*_Max High
      UDS_Message.Data[5] = P2Starmax.Byte.B0;                                        // Loading P2*_Max Low
      TP_TxFrameUSDT('P');                                                            // Sending Response Frame
      return 1;
    }
}
/* ==================================================================================================== */


/* ==================================================================================================== */
/*
 *  UDS ECU Reset
 *
 *  uint8_t UDS_ECUReset (void)
 *
 *  UDS Server Service 0x11 : ECU Reset
 */
/* ---------------------------------------------------------------------------------------------------- */
uint8_t UDS_ECUReset (void) {
    uint8_t _SID = UDS_Message.Data[0];                                               // Extracting SID
    uint8_t _Suppress = (UDS_Message.Data[1] & 0x80) ? 1 : 0;                         // Checking is Positive Response Is Suppressed
    uint8_t _SF = UDS_Message.Data[1] & 0x7F;                                         // Extracting Sub Function

    if ((_SID != 0x01) && (_SID != 0x02) && (_SID != 0x03)) {                         // Checking if Sub Function is Implemented
        TP_SendNegativeResponse(UDS_NRC_SFNS, _SID, 'P');                             // NRC : Sub Function Not Supported
        UDS_Server.Status = UDS_ServerFree;                                           // UDS Server Status Set To Free
        return 0;
    }

    if (!(_Suppress)) {                                                               // Checking if Positive Response is Suppressed
      TP_MessageTX.CANID.Raw = UDS_Server.UDS_TxID;                                   // TP Direct Loading CANID 
      TP_MessageTX.Data[0] = 0x51;                                                    // TP Direct Loading Positive Response
      TP_MessageTX.Data[1] = _SID;                                                    // TP Direct Loading SID
      TP_MessageTX.Data[2] = TP_CANPadding;                                           // TP Direct Loading Padding
      TP_MessageTX.Data[3] = TP_CANPadding;                                           // TP Direct Loading Padding
      TP_MessageTX.Data[4] = TP_CANPadding;                                           // TP Direct Loading Padding
      TP_MessageTX.Data[5] = TP_CANPadding;                                           // TP Direct Loading Padding
      TP_MessageTX.Data[6] = TP_CANPadding;                                           // TP Direct Loading Padding
      TP_MessageTX.Data[7] = TP_CANPadding;                                           // TP Direct Loading Padding
      TP_SendDataFrameCAN();                                                          // Sending Response Frame
    }

    switch (_SF) {
      case 0x01 : {                                                                   // Hard Reset
        // HardReset() Function
        break;
      }
      case 0x02 : {                                                                   // Key ON OFF Reset
        // KeyOffOnReset() Function
        break;
      }
      case 0x03 : {                                                                   // Soft Reset
        // SoftReset() Function
        break;
      }
      default : {
        TP_SendNegativeResponse(UDS_NRC_SFNS, _SID, 'P');                             // NRC : Sub Function Not Supported
        UDS_Server.Status = UDS_ServerFree;                                           // UDS Server Status Set To Free
        return 0;
      }
    }
    UDS_Server.Status = UDS_ServerFree;                                               // UDS Server Status Set To Free
    return 1;
}
/* ==================================================================================================== */


/* ==================================================================================================== */
/*
 *  UDS Tester Present
 *
 *  uint8_t UDS_TesterPresent (void)
 *
 *  UDS Server Service 0x3E : Tester Present
 */
/* ---------------------------------------------------------------------------------------------------- */
uint8_t UDS_TesterPresent (void) {
    uint8_t _SID = UDS_Message.Data[0];                                               // Extracting SID
    uint8_t _Suppress = (UDS_Message.Data[1] & 0x80) ? 1 : 0;                         // Checking is Positive Response Is Suppressed
    uint8_t _SF = UDS_Message.Data[1] & 0x7F;                                         // Extracting Sub Function
    uint32_t _Time = TP_Clock();                                                      // Loaded Current Time
    uint8_t _CurrentSession = UDS_GetSession();                                       // UDS Get Active Current Session
    
    if (_SF != 0x00) {                                                                // Checking Subfunction
        TP_SendNegativeResponse(UDS_NRC_SFNS, _SID, 'P');                             // NRC : Sub Function Not Supported
        UDS_Server.Status = UDS_ServerFree;                                           // UDS Server Status Set To Free
        return 0;
    }

    if (_Suppress) {                                                                  // Checking if Positive Response is Suppressed
        UDS_Server.Status = UDS_ServerFree;                                           // UDS Server Status Set To Free
        return 1;                                                                     // Returning All OK
    } else {                                                                          // Frame Building
      UDS_Message.Length = 2;                                                         // TML Suggested
      UDS_Message.Data[0] = 0x7E;                                                     // Positive Response SID
      UDS_Message.Data[1] = _SF;                                                      // Echo Sub Function
      UDS_Message.Data[2] = TP_CANPadding;
      UDS_Message.Data[3] = TP_CANPadding;
      UDS_Message.Data[4] = TP_CANPadding;
      UDS_Message.Data[5] = TP_CANPadding;
      UDS_Message.Data[6] = TP_CANPadding;
      UDS_Message.Data[7] = TP_CANPadding;
      TP_TxFrameUSDT('P');                                                            // Sending Response Frame
      return 1;
    }
}
/* ==================================================================================================== */









/* ==================================================================================================== */
void UDS_Application (void) {
    // Server Timekeeping
    uint32_t UDS_CurrentTime = TP_Clock();
    UDS_SecurityTimeout(UDS_CurrentTime);                                 // Security Timeout Check
    UDS_SessionTimeout(UDS_CurrentTime);                                  // Session Timeout Check
    //UDS_CommunicationTimeout(UDS_CurrentTime);                          // Communication Control Timeout Check


    if (UDS_Server.Status != UDS_ServerBusy) {
      return;
    }


    uint8_t _SID = UDS_Message.Data[0];

    switch (_SID) {                                                       // Checking SIDs

      // UDS Service : Diagonostic Session Control
      case 0x10 : {                                                       // Diagonostic Session Control Service
        const uint8_t AllowedSecurity = UDS_SecurityNone | UDS_SecurityEnhanced | UDS_SecuritySafety | UDS_SecurityProgramming | UDS_SecurityEOL;
        const uint8_t AllowedSession = UDS_Default | UDS_Extended | UDS_Programming | UDS_Safety | UDS_Engineering;
        const uint8_t AllowedAddressing = UDS_FuncID1 | UDS_FuncID2;

        if (UDS_AddressingCheck(UDS_Message.CANID, AllowedAddressing)) {  // UDS Service Addressing Check
            UDS_Server.Status = UDS_ServerFree;                           // UDS Server Status Set To Free
            return;
        }

        if (UDS_Message.Length != 2) {                                    // Expected Payload Length Check
            TP_SendNegativeResponse(UDS_NRC_IMLIF, _SID, 'P');            // NRC : Incorrect Message Length or Invalid Format
            UDS_Server.Status = UDS_ServerFree;                           // UDS Server Status Set To Free
            return;
        } 

        uint8_t CheckSecurity = UDS_GetSecurity();
        uint8_t CheckSession = UDS_GetSession();

        if ((CheckSession & AllowedSession) != CheckSession) {            // Checking Session
            TP_SendNegativeResponse(UDS_NRC_SNSIAS, _SID, 'P');           // NRC : Service Not Supported In Active Session
            UDS_Server.Status = UDS_ServerFree;                           // UDS Server Status Set To Free
            return;
        }
        if ((CheckSecurity & AllowedSecurity) != CheckSecurity) {         // Checking Security Level
            TP_SendNegativeResponse(UDS_NRC_SAD, _SID, 'P');              // NRC : Security Access Denied
            UDS_Server.Status = UDS_ServerFree;                           // UDS Server Status Set To Free
            return;
        }
            
        uint8_t _Report = 0;
        _Report = UDS_DiagonosticsSessionControl();                       // Call Function For : Diagonostics Session Control
        if (_Report) {                                                    // Check if Valid Service Executed
          UDS_SessionTimerUpdate();                                       // Valid Service, Reseting Server Timer
        }
        return;
      }

      // UDS Service : ECU Reset
      case 0x11 : {                                                       // ECU Reset Service
        const uint8_t AllowedSecurity = UDS_SecurityEnhanced | UDS_SecuritySafety | UDS_SecurityProgramming | UDS_SecurityEOL;
        const uint8_t AllowedSession = UDS_Extended | UDS_Programming | UDS_Safety | UDS_Engineering;
        const uint8_t AllowedAddressing = UDS_FuncID1 | UDS_FuncID2;

        if (UDS_AddressingCheck(UDS_Message.CANID, AllowedAddressing)) {  // UDS Service Addressing Check
            UDS_Server.Status = UDS_ServerFree;                           // UDS Server Status Set To Free
            return;
        }

        if (UDS_Message.Length != 2) {                                    // Expected Payload Length Check
            TP_SendNegativeResponse(UDS_NRC_IMLIF, _SID, 'P');            // NRC : Incorrect Message Length or Invalid Format
            UDS_Server.Status = UDS_ServerFree;                           // UDS Server Status Set To Free
            return;
        } 

        uint8_t CheckSecurity = UDS_GetSecurity();
        uint8_t CheckSession = UDS_GetSession();

        if ((CheckSession & AllowedSession) != CheckSession) {            // Checking Session
            TP_SendNegativeResponse(UDS_NRC_SNSIAS, _SID, 'P');           // NRC : Service Not Supported In Active Session
            UDS_Server.Status = UDS_ServerFree;                           // UDS Server Status Set To Free
            return;
        }
        if ((CheckSecurity & AllowedSecurity) != CheckSecurity) {         // Checking Security Level
            TP_SendNegativeResponse(UDS_NRC_SAD, _SID, 'P');              // NRC : Security Access Denied
            UDS_Server.Status = UDS_ServerFree;                           // UDS Server Status Set To Free
            return;
        }
            
        uint8_t _Report = 0;
        _Report = UDS_ECUReset();                                         // Call Function For : ECU Reset
        if (_Report) {                                                    // Check if Valid Service Executed
          UDS_SessionTimerUpdate();                                       // Valid Service, Reseting Server Timer
        }
        return;
      }

      // UDS Service : Tester Present
      case 0x3E : {                                                       // Tester Present Service
        const uint8_t AllowedSecurity = UDS_SecurityNone | UDS_SecurityEnhanced | UDS_SecuritySafety | UDS_SecurityProgramming | UDS_SecurityEOL;
        const uint8_t AllowedSession = UDS_Default | UDS_Extended | UDS_Programming | UDS_Safety | UDS_Engineering;
        const uint8_t AllowedAddressing = UDS_FuncID1 | UDS_FuncID2;

        if (UDS_AddressingCheck(UDS_Message.CANID, AllowedAddressing)) {  // UDS Service Addressing Check
            UDS_Server.Status = UDS_ServerFree;                           // UDS Server Status Set To Free
            return;
        }

        if (UDS_Message.Length != 2) {                                    // Expected Payload Length Check
            TP_SendNegativeResponse(UDS_NRC_IMLIF, _SID, 'P');            // NRC : Incorrect Message Length or Invalid Format
            UDS_Server.Status = UDS_ServerFree;                           // UDS Server Status Set To Free
            return;
        } 

        uint8_t CheckSecurity = UDS_GetSecurity();
        uint8_t CheckSession = UDS_GetSession();

        if ((CheckSession & AllowedSession) != CheckSession) {            // Checking Session
            TP_SendNegativeResponse(UDS_NRC_SNSIAS, _SID, 'P');           // NRC : Service Not Supported In Active Session
            UDS_Server.Status = UDS_ServerFree;                           // UDS Server Status Set To Free
            return;
        }
        if ((CheckSecurity & AllowedSecurity) != CheckSecurity) {         // Checking Security Level
            TP_SendNegativeResponse(UDS_NRC_SAD, _SID, 'P');              // NRC : Security Access Denied
            UDS_Server.Status = UDS_ServerFree;                           // UDS Server Status Set To Free
            return;
        }
            
        uint8_t _Report = 0;
        _Report = UDS_TesterPresent();                                    // Call Function For : Tester Present
        if (_Report) {                                                    // Check if Valid Service Executed
          UDS_SessionTimerUpdate();                                       // Valid Service, Reseting Server Timer
        }
        return;
      }





      case 0x7F : {
        UDS_Server.Status = UDS_ServerFree;
        return;
      }
      default : {                                                         // Exception Handling
          TP_SendNegativeResponse(UDS_NRC_SNS, _SID, 'P');                // NRC : Service Not Supported
          UDS_Server.Status = UDS_ServerFree;                             // UDS Server Status Set To Free
          return;
      }
    }
}




void UDS_InitApp (void) {
	TP_VariablesStart();
	UDS_VariablesStart();
}



void UDS_MainApp (void) {
    TP_RxDoCAN();
    UDS_Application();
    TP_TxDoCAN();
}





#endif