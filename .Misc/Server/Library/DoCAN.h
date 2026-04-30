/* ==================================================================================================== */
/*
 *  DoCAN.h
 *  Diagonostics on CAN (DoCAN)
 *    ISO: 15765 Part 2   - Diagonostics on CAN
 *  Version: v1.1:1
 */
/* ==================================================================================================== */



#ifndef _DoCAN
#define _DoCAN

#ifdef _UDSonSPI
  #include "UDS.h"
  #include "RachanaUDS.h"
  #include "SansaadhanUDS.h"
#else
  #include "CommonIncs.h"
  #include "UDS.h"
#endif




/* ==================================================================================================== */
/*
 *  Section
 *  Declaring Operational Variables
 *  
 *  Declaring and Initializing the DoCAN Operational Variables
 */
 /* ---------------------------------------------------------------------------------------------------- */
#ifndef TP_CANPadding
  #define TP_CANPadding                               0x55                            // TP CAN Padding Character
#endif

#ifndef TP_RxBufferSize
  #define TP_RxBufferSize                             128u                            // TP Receive Message Buffer Size
#endif

#ifndef TP_FlowStatus
  #define TP_FlowStatus
  #define TP_FSContinueToSend                         0x00                            // TP FS Continue To Send (CTS)
  #define TP_FSWait                                   0x01                            // TP FS Wait (WT)
  #define TP_FSOverflow                               0x02                            // TP FS Overflow (OF)
#endif

#ifndef TP_ServerParameters
  #define TP_ServerParameters
  #define TP_ServerWaitCountDown                      600u                            // TP Wait Count Down
  #define TP_ServerWaitTimeout                        5000u                           // TP Wait Timeout
  #define TP_Server_NAs                               1000u                           // TP Timeout N_As
  #define TP_Server_NAr                               1000u                           // TP Timeout N_Ar
  #define TP_Server_NBs                               1000u                           // TP Timeout N_Bs
  #define TP_Server_NBr                               0u                              // TP Timeout N_Br
  #define TP_Server_NCs                               0u                              // TP Timeout N_Cs
  #define TP_Server_NCr                               1000u                           // TP Timeout N_Cr
#endif

#ifndef TP_TxProcessFlow
  #define TP_TxProcessFlow
  #define TP_TxProcessIdle                            0u
  #define TP_TxProcessFFSending                       1u
  #define TP_TxProcessFCWait                          2u
  #define TP_TxProcessSeparationWait                  3u
  #define TP_TxProcessCFSending                       4u
  #define TP_TxProcessWaiting                         5u
  #define TP_TxProcessSFSending                       6u
#endif


typedef union {
    uint8_t Raw;
    struct {
      uint8_t B0;
    } Byte;
    struct {
      uint8_t N0 : 4;
      uint8_t N1 : 4;
    } Nibble;
} Number8Bit;

typedef union {
    uint16_t Raw;
    struct {
      uint8_t B0;
      uint8_t B1;
    } Byte;
    struct {
      uint8_t N0 : 4;
      uint8_t N1 : 4;
      uint8_t N2 : 4;
      uint8_t N3 : 4;
    } Nibble;
} Number16Bit;

typedef union {
    uint32_t Raw;
    struct {
      uint16_t W0;
      uint16_t W1;
    } Word;
    struct {
      uint8_t B0;
      uint8_t B1;
      uint8_t B2;
      uint8_t B3;
    } Byte;
} Number32Bit;

typedef union {
    uint64_t Raw;
    struct {
      uint32_t I0;
      uint32_t I1;
    } Integer;
    struct {
      uint16_t W0;
      uint16_t W1;
      uint16_t W2;
      uint16_t W3;
    } Word;
    struct {
      uint8_t B0;
      uint8_t B1;
      uint8_t B2;
      uint8_t B3;
      uint8_t B4;
      uint8_t B5;
      uint8_t B6;
      uint8_t B7;
    } Byte;
} Number64Bit;


// TP CAN Message
typedef struct {
    Number16Bit CANID;                                                                // TP CAN ID
    uint8_t Flag;                                                                     // TP CAN Flag (Active High)
    uint8_t Data[8];                                                                  // TP CAN Data
} TP_CANMessage;
extern TP_CANMessage TP_MessageRX;
extern TP_CANMessage TP_MessageTX;

// TP Status
typedef struct {
    uint8_t RxFlag;                                                                   // TP Receive Flag (Active High)
    uint8_t TxFlag;                                                                   // TP Transmit Flag (Active High)
    uint16_t WaitCount;                                                               // TP CAN Wait Counter
} TP_StatusLayer;
extern TP_StatusLayer TP_Status;

// TP Receive Manager
typedef struct {
    uint8_t OverflowFlag;                                                             // TP Buffer Overflow Flag (Active High)
    uint8_t FrameIndex;                                                               // TP Consecutive Frame
    uint16_t FrameCounter;                                                            // TP Consecutive Frame Counter
    uint16_t DataCounter;                                                             // TP Data Bytes Counter
    uint16_t TotalLength;                                                             // TP Total Length of Data in Bytes
    uint16_t TotalFrames;                                                             // TP Total Numbers of Consecutive Frames
} TP_SegmentedBlockRx;
extern TP_SegmentedBlockRx TP_RxControl;

typedef struct {
    uint32_t Time;                                                                    // TP Buffer Entry Time
    uint8_t Process;                                                                  // TP Buffer Process Flow
    uint8_t FlowStatus;                                                               // TP Buffer Tester Bus State
    uint8_t FrameIndex;                                                               // TP Consecutive Frame
    uint16_t FrameCounter;                                                            // TP Consecutive Frame Counter
    uint16_t TotalFrames;                                                             // TP Total Numbers of Consecutive Frames
    uint16_t FramesAllowed;                                                           // Tp Frames Allowed To Sent
    uint16_t DataCounter;                                                             // TP Data Bytes Counter
    uint32_t SeparationTimeout;                                                       // TP Separation Timeout 
    uint16_t BlocksAllowed;                                                           // TP Number of Blocks Left
    uint8_t ReceivedFC;                                                               // TP Received Flow Control Flag
} TP_SegmentedBlockTx;
extern TP_SegmentedBlockTx TP_TxControl;
/* ==================================================================================================== */


extern uint32_t TP_Clock (void);
extern void TP_ReceiveDataCAN (uint16_t _CANID, uint8_t _D0, uint8_t _D1, uint8_t _D2,
      uint8_t _D3, uint8_t _D4, uint8_t _D5, uint8_t _D6, uint8_t _D7);
extern void TP_SendDataCAN (uint16_t _CANID, uint8_t _D0, uint8_t _D1, uint8_t _D2,
      uint8_t _D3, uint8_t _D4, uint8_t _D5, uint8_t _D6, uint8_t _D7);
extern void TP_SendDataFrameCAN (void);

extern void TP_VariablesStart (void);
extern void TP_SendNegativeResponse (uint8_t _Reason, uint8_t _SID, char C);
extern uint8_t TP_CheckCANID (uint16_t _CANID, char C);

extern void TP_RxFrameSF (void);
extern void TP_RxFrameFF (void);
extern void TP_RxFrameCF (void);
extern void TP_RxFrameFC (void);

extern void TP_TxFrameSF (void);
extern void TP_TxFrameFF (void);
extern void TP_TxFrameCF (void);
extern void TP_TxFrameFC (uint16_t CANID, uint8_t FS, uint8_t STmin, uint8_t BS);

extern void TP_RxDoCAN (void);
extern void TP_TxDoCAN (void);
extern void TP_TxFrameUSDT (char C);


#ifdef _UDSonSPI
  #include "UDSonSPI.h"                                                               // Physical Layer Included
#endif











/* ==================================================================================================== */
/*
 *  DoCAN.c
 *  Diagonostics on CAN (DoCAN)
 *    ISO: 15765 Part 2   - Diagonostics on CAN
 *  Version: v1.1:1
 */
/* ==================================================================================================== */


#ifdef _UDSonSPI
  #include "UDS.h"
#else
  #include "CommonIncs.h"
  #include "DoCAN.h"
  #include "UDS.h"
#endif




TP_StatusLayer TP_Status = {0};
TP_CANMessage TP_MessageRX = {0};
TP_CANMessage TP_MessageTX = {0};
TP_SegmentedBlockRx TP_RxControl = {0};
TP_SegmentedBlockTx TP_TxControl = {0};


/* ==================================================================================================== */
/*
 *  Section
 *  Sending & Receiving CAN Message
 *
 *  Connecting DoCAN and UDS Layer to Physical Layer
 *
 *  uint32_t TP_Clock (void)
 *  void TP_ReceiveDataCAN (uint16_t _CANID, uint8_t _D0, uint8_t _D1, uint8_t _D2,
 *        uint8_t _D3, uint8_t _D4, uint8_t _D5, uint8_t _D6, uint8_t _D7)
 *  void TP_SendDataCAN (uint16_t _CANID, uint8_t _D0, uint8_t _D1, uint8_t _D2, uint8_t _D3,
 *        uint8_t _D4, uint8_t _D5, uint8_t _D6, uint8_t _D7)
 *  void TP_SendDataFrameCAN (void)
 */
/* ---------------------------------------------------------------------------------------------------- */
uint32_t TP_Clock (void) {
#ifdef _UDSonSPI
    return Tools.Clock(); 
#else
    return xTaskGetTickCount();
#endif
}
/* ---------------------------------------------------------------------------------------------------- */
void TP_ReceiveDataCAN (uint16_t _CANID, uint8_t _D0, uint8_t _D1, uint8_t _D2,
      uint8_t _D3, uint8_t _D4, uint8_t _D5, uint8_t _D6, uint8_t _D7) {
    TP_MessageRX.CANID.Raw = _CANID;                                                  // TP CAN ID Receoved Loaded
    TP_MessageRX.Data[0] = _D0;                                                       // TP CAN Data Received Loaded
    TP_MessageRX.Data[1] = _D1;                                                       // TP CAN Data Received Loaded
    TP_MessageRX.Data[2] = _D2;                                                       // TP CAN Data Received Loaded
    TP_MessageRX.Data[3] = _D3;                                                       // TP CAN Data Received Loaded
    TP_MessageRX.Data[4] = _D4;                                                       // TP CAN Data Received Loaded
    TP_MessageRX.Data[5] = _D5;                                                       // TP CAN Data Received Loaded
    TP_MessageRX.Data[6] = _D6;                                                       // TP CAN Data Received Loaded
    TP_MessageRX.Data[7] = _D7;                                                       // TP CAN Data Received Loaded
    TP_Status.RxFlag = 0x01;                                                          // TP CAN Receive Flag Set
}
/* ---------------------------------------------------------------------------------------------------- */
void TP_SendDataCAN (uint16_t _CANID, uint8_t _D0, uint8_t _D1, uint8_t _D2,
      uint8_t _D3, uint8_t _D4, uint8_t _D5, uint8_t _D6, uint8_t _D7) {
#ifdef _UDSonSPI
    SPI_TransmitFrameBuild(_CANID, _D0, _D1, _D2, _D3, _D4, _D5, _D6, _D7);           // SPI Transmit Frame Building
    SPI_TransmitFrameShow();                                                          // SPI Transmit Frame Show
    SPI_TransmitFrame();                                                              // SPI Transmit Frame Sent
#else
	CanData_t SendData;
	SendData.Id = _CANID;
	SendData.DLC = 8;
	SendData.Data[0] = _D7;
	SendData.Data[1] = _D6;
	SendData.Data[2] = _D5;
	SendData.Data[3] = _D4;
	SendData.Data[4] = _D3;
	SendData.Data[5] = _D2;
	SendData.Data[6] = _D1;
	SendData.Data[7] = _D0;
	SendCanMessage(SendData);
#endif
}
/* ---------------------------------------------------------------------------------------------------- */
void TP_SendDataFrameCAN (void) {
    TP_SendDataCAN(TP_MessageTX.CANID.Raw, TP_MessageTX.Data[0], TP_MessageTX.Data[1],// TP Sent Data to CAN
                    TP_MessageTX.Data[2], TP_MessageTX.Data[3], TP_MessageTX.Data[4],
                    TP_MessageTX.Data[5], TP_MessageTX.Data[6], TP_MessageTX.Data[7]);
}
/* ==================================================================================================== */





/* ==================================================================================================== */
/*
 *  Section
 *  Misc Functions
 *
 *  Misc Functions for TP Layers
 *
 *  uint8_t TP_CheckCANID (uint16_t _CANID, char C)
 *  void TP_SendNegativeResponse (uint8_t _Reason, uint8_t _SID, uint16_t _CANID)
 */
/* ---------------------------------------------------------------------------------------------------- */
uint8_t TP_CheckCANID (uint16_t _CANID, char C) {
    if ((C == 'R') || (C == 'r')) {                                                   // Received Mode Selection
        if (_CANID == UDS_Server.UDS_RxID) {                                          // Received CAN IDs Check
          return 0;                                                                   // Received Physical CAN ID Flag
        }
        uint8_t i = 0;
        while (i < UDS_Addressing.FunctionalIDAvailable) {                            // Received Functional Address ID Check Loop
          if (_CANID == UDS_Addressing.FunctionalRxID[i]) {                           // Received Functional Address ID Check
            return 0;                                                                 // Received Functional ID Flag
          }
          i++;
        }
        return 0xFF;                                                                  // No CAN ID Matched
    }
    if ((C == 'T') || (C == 't')) {                                                   // Transmited Mode Selection
        if (_CANID == UDS_Server.UDS_TxID) {                                          // Transmited CAN IDs Check
          return 0;                                                                   // Transmited Physical CAN ID Flag
        }
        uint8_t i = 0;
        while (i < UDS_Addressing.FunctionalIDAvailable) {                            // Transmited Functional Address ID Check Loop
          if (_CANID == UDS_Addressing.FunctionalTxID[i]) {                           // Transmited Functional Address ID Check
            return 0;                                                                 // Transmited Functional ID Flag
          }
          i++;
        }
        return 0xFF;                                                                  // No CAN ID Matched
    }
    return 0xFF;
}
/* ---------------------------------------------------------------------------------------------------- */
void TP_SendNegativeResponse (uint8_t _Reason, uint8_t _SID, char C) {
    if ((C == 'P') || (C == 'p')) {                                                   // For Physical Addressing of UDS
        TP_SendDataCAN(UDS_Server.UDS_TxID, UDS_NRC_Length, UDS_NRC, _SID, _Reason,   // Sending Negative Response
                  TP_CANPadding, TP_CANPadding, TP_CANPadding, TP_CANPadding);
    } else if ((C == 'F') || (C == 'f')) {                                            // For Functional Addressing of UDS
        // Functional Addressing
    }
}
/* ---------------------------------------------------------------------------------------------------- */
void TP_VariablesStart (void) {
    // Global Variable : TP_MessageRX & TP_MessageTX
    TP_MessageRX.CANID.Raw = 0u;                                                      // TP Message Rx CAN ID
    TP_MessageTX.CANID.Raw = 0u;                                                      // TP Message Tx CAN ID
    TP_MessageRX.Flag = 0u;                                                           // TP Message Rx Flag (Active High)
    TP_MessageTX.Flag = 0u;                                                           // TP Message Tx Flag (Active High)
    for (uint8_t i = 0; i < 8; i++) {
      TP_MessageRX.Data[i] = 0u;                                                      // TP Rx Data Initialization
      TP_MessageTX.Data[i] = 0u;                                                      // TP Tx Data Initialization
    }

    // Global Variable : TP_Status
    TP_Status.RxFlag = 0u;                                                            // TP Status Rx Flag (Active High)
    TP_Status.TxFlag = 0u;                                                            // TP Status Tx Flag (Active High)
    TP_Status.WaitCount = 0u;                                                         // TP Status Wait Counter

    // Global Variable : TP_RxControl
    TP_RxControl.DataCounter = 0u;                                                    // TP Rx Data Byte Counter
    TP_RxControl.FrameCounter = 0u;                                                   // TP Rx Frame Counter
    TP_RxControl.TotalLength = 0u;                                                    // TP Rx Total Length in Bytes
    TP_RxControl.TotalFrames = 0u;                                                    // TP Rx Total Frames
    TP_RxControl.FrameIndex = 0u;                                                     // TP Rx Consecutive Frame Index
    TP_RxControl.OverflowFlag = 0u;                                                   // TP Rx Buffer Overflow

    // Global Variable : TP_TxControl
    TP_TxControl.FlowStatus = 0u;                                                     // TP Tx Flow Status Received From Flow Control
    TP_TxControl.ReceivedFC = 0u;                                                     // TP Tx Received Flow Control Flag (Active High)
    TP_TxControl.FrameIndex = 0u;                                                     // TP Tx Consecutive Frame Index
    TP_TxControl.FrameCounter = 0u;                                                   // TP Tx Frame Counter
    TP_TxControl.FramesAllowed = 0u;                                                  // TP Tx Frames Allowed Untill New Flow Control
    TP_TxControl.TotalFrames = 0u;                                                    // TP Tx Total Frames in the Data
    TP_TxControl.DataCounter = 0u;                                                    // TP Tx Data Bytes Counter
    TP_TxControl.BlocksAllowed = 0u;                                                  // TP Tx Blocks Allowed Received From Flow Control
    TP_TxControl.SeparationTimeout = 0u;                                              // TP Tx Separation Time Received From Flow Control
    TP_TxControl.Time = 0u;                                                           // TP Tx Timer Entry Points
    TP_TxControl.Process = TP_TxProcessIdle;                                          // TP Tx Process Set to Idle
}

extern void TP_SendFlowControl (void);
void TP_SendFlowControl (void) {
    // Any Logic For Processing and Sending Flow Control
    TP_TxFrameFC(UDS_Server.UDS_TxID,0,0,0);                                          // TP Transmit Flow Control
}
/* ==================================================================================================== */





/* ==================================================================================================== */
/*
 *  Section
 *  Receive Data Frame
 *
 *  Receive Data in DoCAN Layer
 *
 *  void TP_RxFrameSF (void)
 *  void TP_RxFrameFF (void)
 *  void TP_RxFrameCF (void)
 *  void TP_RxFrameFC (void)
 */
/* ---------------------------------------------------------------------------------------------------- */
void TP_RxFrameSF (void) {
    if (UDS_Server.Status == UDS_ServerFree) {                                        // Checking if UDS Server is Free
      if ((TP_MessageRX.Data[0] < 8) && (TP_MessageRX.Data[0] != 0)) {                // Checking for Length
          UDS_Message.CANID = TP_MessageRX.CANID.Raw;                                 // UDS CAN ID Loaded
          UDS_Message.Length = TP_MessageRX.Data[0];                                  // Frame Length Loaded
          uint8_t i = 0;
          while (i < UDS_Message.Length) {                                            // Checking Length
            UDS_Message.Data[i] = TP_MessageRX.Data[i + 1];                           // Loading Data To UDS
            i++;
          }
          UDS_Server.Status = UDS_ServerBusy;                                         // UDS Server Status Set Busy
      } else {
        TP_SendNegativeResponse(UDS_NRC_IMLIF, TP_MessageRX.Data[1], 'P');            // NRC : Incorrect Format
      }
    } else {
        TP_SendNegativeResponse(UDS_NRC_BRR, TP_MessageRX.Data[1], 'P');              // NRC : Busy, Repeat Request
    }
}
/* ---------------------------------------------------------------------------------------------------- */
void TP_RxFrameFF (void) {
    if (UDS_Server.Status == UDS_ServerFree) {                                        // Checking if UDS Server is Free
        uint16_t Length = 0;
        Length = (uint16_t)(TP_MessageRX.Data[1] |                                    // Extracting Length
                    (uint16_t)((TP_MessageRX.Data[0] & 0x0F) << 8));
        if ((Length > 7) && (Length < 4096)) {                                        // Checking for Length
            UDS_Message.CANID = TP_MessageRX.CANID.Raw;                               // UDS CAN ID Loaded
            UDS_Message.Length = Length;                                              // UDS Frame Length Loaded
            TP_RxControl.TotalLength = Length;                                         // TP Receive Total Payload Loaded
            uint16_t Frames = 0;
            Frames = Length - 6;
            if ((Frames % 7) == 0) {
                Frames = (Frames / 7) + 1;
            } else {
                Frames = (Frames / 7) + 2;
            }
            TP_RxControl.TotalFrames = Frames;                                         // TP Receive Total Number of Frames
            TP_RxControl.FrameCounter = 1;                                             // TP Receive Frame Counter Set To 1
            TP_RxControl.FrameIndex = 1;                                               // TP Receive Frame Index is Zero + 1
            TP_Status.WaitCount = 0;                                                  // TP Receive Status Wait Count Reset
            TP_RxControl.DataCounter = 0;                                              // TP Receive Data Byte Counter Reseted
            uint8_t i = 2;
            while (i < 8) {
              UDS_Message.Data[i - 2] = TP_MessageRX.Data[i];                         // TP Received Frames Initially Loaded in UDS
              TP_RxControl.DataCounter++;                                              // TP Received Data Bytes Counter Incremented
              i++;
            }
            UDS_Server.Status = UDS_ServerReceiving;                                  // UDS Server in Receiving Mode
            
        } else {
            TP_SendNegativeResponse(UDS_NRC_IMLIF, TP_MessageRX.Data[1], 'P');        // NRC : Incorrect Format
        }
    } else {
        TP_SendNegativeResponse(UDS_NRC_BRR, TP_MessageRX.Data[1], 'P');              // NRC : Busy, Repeat Request
    }
}
/* ---------------------------------------------------------------------------------------------------- */
void TP_RxFrameCF (void) {
    if (UDS_Server.Status == UDS_ServerReceiving) {                                   // Checking if UDS Server is Receiving Mode
        uint8_t FrameIndex = 0;
        FrameIndex = 0x0F & TP_MessageRX.Data[0];                                     // Frame Index Extraction
        if (TP_RxControl.FrameIndex == FrameIndex) {                                   // Frame Index Checker
            TP_RxControl.FrameIndex = (TP_RxControl.FrameIndex + 1) % 16;               // TP Receive Frame Index Incremented with Overflow Check
            TP_RxControl.FrameCounter++;                                               // TP Frame Counter Incremented
            uint8_t i = 1;
            while (i < 8) {
              if (TP_RxControl.DataCounter >= TP_RxBufferSize) {                       // TP Data Counter Check for Memory Check
                  break;                                                              // TP Rare Case Handling
              }
              UDS_Message.Data[TP_RxControl.DataCounter] = TP_MessageRX.Data[i];       // TP To UDS Data Loading
              TP_RxControl.DataCounter++;                                              // TP Received Data Bytes Counter Incremented
              i++;
            }

            if (TP_RxControl.FrameCounter >= TP_RxControl.TotalFrames) {                // TP Receiver Check is All Frames Received
              UDS_Server.Status = UDS_ServerBusy;                                     // UDS Server Status is Set To Busy``
            }
        }
    }
}
/* ---------------------------------------------------------------------------------------------------- */
void TP_RxFrameFC (void) {
    if ((UDS_Server.Status == UDS_ServerTransmitting) ||                              // Checking if UDS Server is Transmitting Mode and Waiting Mode
          (UDS_Server.Status == UDS_ServerWaiting)) {
      uint8_t Status = 0;
      Status = TP_MessageRX.Data[0] & 0x0F;                                           // Extracting Flow Status (FS)
      switch (Status) {
        case TP_FSContinueToSend : {                                                  // FS = Continue To Send (CTS)
            TP_TxControl.FlowStatus = TP_FSContinueToSend;                               // TP Transmit Status To CTS
            TP_TxControl.BlocksAllowed = TP_MessageRX.Data[1];                         // Extracting Block Size (BS) and Loading in TP
            uint32_t _Time = 0;
            if ((TP_MessageRX.Data[2] >> 4) < 10)  {                                  // Checking For Format of STMin
              _Time = (TP_MessageRX.Data[2] & 0xF0) ? 1 : 0;                          // Assigning 1 ms
            }
            _Time = (TP_MessageRX.Data[2] & 0x0F) + _Time;                            // Extracting Separation Time Minimum (STMin)
            TP_TxControl.SeparationTimeout = _Time;                                    // Loading Separation Time Minimum
            TP_TxControl.ReceivedFC = 1;                                               // TP Received Flow Control Flag is Set
            break;
        }
        case TP_FSWait : {                                                            // FS = Wait (WT)
            TP_TxControl.FlowStatus = TP_FSWait;                                         // TP Transmit Status To Wait
            UDS_Server.Status = UDS_ServerWaiting;                                    // UDS Server Status is Set To Waiting
            TP_TxControl.ReceivedFC = 1;                                               // TP Received Flow Control Flag is Set
            break;
        }
        case TP_FSOverflow : {                                                        // FS = Overflow Flag (OF)
            // Overflow Functions
            TP_TxControl.FlowStatus = TP_FSOverflow;                                     // TP Transmit Status To Overflow Flag
            UDS_Server.Status = UDS_ServerFree;                                       // UDS Server Status is Set To Free
            TP_TxControl.ReceivedFC = 1;                                               // TP Received Flow Control Flag is Set
            break;
        }
        default : {
            TP_SendNegativeResponse(UDS_NRC_IMLIF, TP_MessageRX.Data[1], 'P');        // NRC : Incorrect Format
            UDS_Server.Status = UDS_ServerFree;                                       // UDS Server Status is Set To Free
            break;
        }
      }
    }
}
/* ==================================================================================================== */





/* ==================================================================================================== */
/*
 *  Section
 *  Transmit Data Frame
 *
 *  Transmit Data in DoCAN Layer
 *
 *  void TP_TxFrameSF (void)
 *  void TP_TxFrameFF (void)
 *  void TP_TxFrameCF (void)
 *  void TP_TxFrameFC (void)
 */
/* ---------------------------------------------------------------------------------------------------- */
void TP_TxFrameSF (void) {
    if (UDS_Message.Length < 8) {                                                     // Message Length Check
      TP_MessageTX.Data[0] = UDS_Message.Length;                                      // Loading Payload Length
      uint8_t i = 1;
      while (i < (UDS_Message.Length + 1)) {                                          // Loop For Loading Payload
        TP_MessageTX.Data[i] = UDS_Message.Data[i - 1];                               // Loading Payload
        i++;
      }
      while (i < 8) {                                                                 // Loop For Padding
        TP_MessageTX.Data[i] = TP_CANPadding;                                         // Doing Padding
        i++;
      }
      TP_SendDataFrameCAN();                                                          // Sending Data via CAN
      UDS_Server.Status = UDS_ServerFree;                                             // UDS Server Status is Set To Free
    } else {
      UDS_Server.Status = UDS_ServerFree;                                             // UDS Server Status is Set To Free
      return;
    }
}
/* ---------------------------------------------------------------------------------------------------- */
void TP_TxFrameFF (void) {
    Number16Bit Length;
    Length.Raw = UDS_Message.Length;
    TP_MessageTX.Data[0] = 0x10 | (0x0F & Length.Byte.B1);                            // UDS First Frame PCI and Payload Length Loaded
    TP_MessageTX.Data[1] = Length.Byte.B0;                                            // UDS Payload Length Loaded
    uint8_t i = 2;
    while (i < 8) {
      TP_MessageTX.Data[i] = UDS_Message.Data[i - 2];                                 // UDS Message Loading
      TP_TxControl.DataCounter++;                                                     // UDS Transmit Data Counter Incremented
      i++;
    }
    TP_TxControl.FrameIndex = 1;                                                      // UDS Frame Index Start At 0 Incremented to 1
    TP_SendDataFrameCAN();                                                            // Sending Data via CAN
}
/* ---------------------------------------------------------------------------------------------------- */
void TP_TxFrameCF (void) {
    TP_MessageTX.Data[0] = 0x20 | TP_TxControl.FrameIndex;                            // UDS Loaded Consecutive Frame PCI
    TP_TxControl.FrameIndex = (TP_TxControl.FrameIndex + 1) % 16;                     // UDS Frame Index Increamented
    uint8_t i = 1;
    while (i < 8) {
        if (TP_TxControl.DataCounter >= UDS_Message.Length) {                         // UDS Data Length Checker
            break;
        }
        TP_MessageTX.Data[i] = UDS_Message.Data[TP_TxControl.DataCounter];            // UDS Data Loaded
        TP_TxControl.DataCounter++;                                                   // UDS Data Byte Counter Increamented
        i++;
    }
    while (i < 8) {
        TP_MessageTX.Data[i] = TP_CANPadding;                                         // UDS Padding for Rest of CAN Frame
        i++;
    }
    TP_SendDataFrameCAN();                                                            // Sending Data via CAN
}
/* ---------------------------------------------------------------------------------------------------- */
void TP_TxFrameFC (uint16_t CANID, uint8_t FS, uint8_t STmin, uint8_t BS) {
    TP_MessageTX.CANID.Raw = CANID;                                                   // CAN ID Loaded
    TP_MessageTX.Data[0] = 0x30 | (0x03 & FS);                                        // PCI and Flow Status Loaded
    TP_MessageTX.Data[1] = BS;                                                        // Block Size Loaded
    TP_MessageTX.Data[2] = STmin;                                                     // Speration Time Loaded
    TP_MessageTX.Data[3] = TP_CANPadding;                                             // CAN Padding Loaded
    TP_MessageTX.Data[4] = TP_CANPadding;                                             // CAN Padding Loaded
    TP_MessageTX.Data[5] = TP_CANPadding;                                             // CAN Padding Loaded
    TP_MessageTX.Data[6] = TP_CANPadding;                                             // CAN Padding Loaded
    TP_MessageTX.Data[7] = TP_CANPadding;                                             // CAN Padding Loaded
    TP_SendDataFrameCAN();                                                            // Sending Data via CAN
}
/* ---------------------------------------------------------------------------------------------------- */
void TP_TxFrameUSDT (char C) {
    if ((C == 'P') || (C == 'p')) {                                                   // Physical Addressing Mode
      TP_MessageTX.CANID.Raw = UDS_Server.UDS_TxID;                                   // Loaded Physical Addressing TX CAN ID
    } else if ((C == 'F') || (C == 'f')) {                                            // Functional Addressing Mode
      // Functional IDs
    } else {                                                                          // Ignoring Exceptions
      UDS_Server.Status = UDS_ServerFree;                                             // UDS Server Status is Set To Free
      TP_TxControl.Process = TP_TxProcessIdle;                                        // TP Process Selected To Idle State
      return;                                                                         // Doing Nothing
    }

    // Single Frame Transmission
    if (UDS_Message.Length < 8) {                                                     // UDS Segamented Single Transmission
      TP_Status.TxFlag = 1;                                                           // TP TX Flag Is Set
      UDS_Server.Status = UDS_ServerTransmitting;                                     // UDS Server Status is Set To Transmitting
      TP_TxControl.Process = TP_TxProcessSFSending;                                   // TP Process Selected To Single Frame State
      return;
    }

    // Multi Frame Transmission
    else {                                                                            // UDS Segamented Multiple Transmission
      if (UDS_Message.Length >= UDS_ParaBufferSize) {                                 // Normal USDT Sending Check
        UDS_Server.Status = UDS_ServerFree;                                           // UDS Server Status is Set To Free
        TP_TxControl.Process = TP_TxProcessIdle;                                      // TP Process Selected To Idle State
        return;                                                                       // Doing Nothing
      }
      UDS_Server.Status = UDS_ServerTransmitting;                                     // UDS Server Status is Set To Transmitting
      uint16_t Frames = 0;
      Frames = UDS_Message.Length - 6;                                                // Number of Frames Check
      if ((Frames % 7) == 0) {
          Frames = (Frames / 7) + 1;                                                  // Frames Require No Padding At Last
      } else {
          Frames = (Frames / 7) + 2;                                                  // Frames Require Padding At Last
      }
      TP_Status.WaitCount = 0;
      TP_TxControl.TotalFrames = Frames;                                              // TP Total Frames Loaded
      TP_TxControl.FrameCounter = 0;
      TP_TxControl.FrameIndex = 0;
      TP_TxControl.FramesAllowed = 0;
      TP_TxControl.DataCounter = 0;
      TP_TxControl.ReceivedFC = 0;
      TP_TxControl.BlocksAllowed = 0;                                                 // ISO Assumptions
      TP_TxControl.SeparationTimeout = 0;                                             // ISO Assumptions
      TP_TxControl.FlowStatus = TP_FSContinueToSend;                                  // ISO Assumptions
      TP_Status.TxFlag = 1;                                                           // TP TX Flag Is Set
      TP_TxControl.Process = TP_TxProcessFFSending;                                   // TP Process Selected To First Frame State
    }
}
/* ---------------------------------------------------------------------------------------------------- */









void TP_TxDoCAN (void) {
    if (TP_Status.TxFlag == 0x00) {                                                   // Checking for TX Flag
      return;
    }
    else if (TP_Status.TxFlag == 0x01) {                                              // If CAN Message Transmission
      uint32_t Time = TP_Clock();                                                     // Loading Clock Time
      switch (TP_TxControl.Process) {                                                 // Selecting Transmission Process

        case TP_TxProcessIdle : {                                                     // TP Process : Idle State
            TP_Status.TxFlag = 0x00;                                                  // TP Reseting TX Flag
            return;
        }

        case TP_TxProcessFFSending : {                                                // TP Process : Sending First Frame State
            TP_TxFrameFF();                                                           // Sending First Frame
            TP_TxControl.FrameCounter++;                                              // TP Frame Counter Incremented
            TP_TxControl.Process = TP_TxProcessFCWait;                                // TP Process Selected To FF Sent
            TP_TxControl.Time = TP_Clock();                                           // TP Flow Control Wait Entry Time
            return;
        }

        case TP_TxProcessFCWait : {                                                   // TP Process : Flow Control Wait State
            if ((Time - TP_TxControl.Time) > TP_Server_NBs) {                         // Flow Control Receive Timeout
                UDS_Server.Status = UDS_ServerFree;                                   // UDS Server Status is Set To Free
                TP_TxControl.Process = TP_TxProcessIdle;                              // TP Process Selected To Idle State
                return;
            }
            if (TP_TxControl.ReceivedFC == 0x01) {                                    // Check if Flow Control Received
              if (TP_TxControl.FlowStatus == TP_FSOverflow) {                         // TP Flow Status Overflow Flag
                  UDS_Server.Status = UDS_ServerFree;                                 // UDS Server Status is Set To Free
                  TP_TxControl.Process = TP_TxProcessIdle;                            // TP Process Selected To Idle State
                  return;
              } else if (TP_TxControl.FlowStatus == TP_FSWait) {                      // TP Flow Status Wait
                  TP_Status.WaitCount++;                                              // TP Wait Counter Increamented
                  UDS_Server.Status = UDS_ServerWaiting;                              // UDS Server Status is Set To Waiting
                  if (TP_Status.WaitCount >= TP_ServerWaitCountDown) {                // Wait Counter Count Out Check
                      UDS_Server.Status = UDS_ServerFree;                             // UDS Server Status is Set To Free
                      TP_TxControl.Process = TP_TxProcessIdle;                        // TP Process Selected To Idle State
                      return;                                                         // TP Wait Counted Out
                  }
                  TP_TxControl.Process = TP_TxProcessWaiting;                         // TP Process Selected To Waiting State
                  TP_TxControl.Time = TP_Clock();                                     // TP Wait Entry Time Loaded
                  return;
              } else if (TP_TxControl.FlowStatus == TP_FSContinueToSend) {
                  if (TP_TxControl.BlocksAllowed == 0) {
                    TP_TxControl.FramesAllowed = TP_TxControl.TotalFrames             // TP Allowing Rest All Frames
                        - TP_TxControl.FrameCounter;
                  } else {
                    uint16_t BS = 0;
                    BS = TP_TxControl.TotalFrames - TP_TxControl.FrameCounter;        // TP Allowing Only BS Blocks Only
                    if (TP_TxControl.BlocksAllowed >= BS) {                           // If Blocks Allowed as More Than Data in Buffer
                      TP_TxControl.FramesAllowed = BS;                                // TP Allowing Rest All Frames
                    } else {
                      TP_TxControl.FramesAllowed = TP_TxControl.BlocksAllowed;        // TP Allowing Only Requested Frames
                    }
                  }
                  UDS_Server.Status = UDS_ServerTransmitting;                         // UDS Server Status is Set To Transmitting
                  TP_TxControl.Process = TP_TxProcessSeparationWait;                  // TP Process Selected To Frame Separation Wait State
                  TP_TxControl.Time = TP_Clock();                                     // TP Separation Time Entry Time Loaded
              }
              TP_TxControl.ReceivedFC = 0x00;                                         // TP Reseting Flow Control Flag
            }
            return;
        }

        case TP_TxProcessSeparationWait : {                                           // TP Process : Separation Timeout Between Frames State
            if ((Time - TP_TxControl.Time) > TP_TxControl.SeparationTimeout) {        // Separation Between Frame Timeout
                TP_TxControl.Process = TP_TxProcessCFSending;                         // TP Process Selected To Send Consecutive Frame State
            }
            return;
        }

        case TP_TxProcessCFSending : {                                                // TP Process : Sending Consecutive Frame State
            if (TP_TxControl.FramesAllowed) {                                         // If Frames Are Allowed
                TP_TxFrameCF();                                                       // TP Sending Consecutive Frame
                TP_TxControl.FrameCounter++;                                          // TP Frame Counter Incremented
                TP_TxControl.FramesAllowed--;                                         // TP Frame Allowed Decremented
                if (TP_TxControl.FrameCounter >= TP_TxControl.TotalFrames) {          // Checking if All Frames Transmitted
                    UDS_Server.Status = UDS_ServerFree;                               // UDS Server Status is Set To Free
                    TP_TxControl.Process = TP_TxProcessIdle;                          // TP Process Selected To Idle State
                    return;
                }
                TP_TxControl.Time = TP_Clock();                                       // TP Separation Time Entry Time Loaded
                TP_TxControl.Process = TP_TxProcessSeparationWait;                    // TP Process Selected To Frame Separation Wait State
            } else {
                TP_TxControl.Process = TP_TxProcessFCWait;                            // TP Process Selected To Wait for Flow Control State
            }
            return;
        }

        case TP_TxProcessWaiting : {                                                  // TP Process : Wait State
            if ((Time - TP_TxControl.Time) > TP_ServerWaitTimeout) {                  // Wait Receive Timeout
                UDS_Server.Status = UDS_ServerFree;                                   // UDS Server Status is Set To Free
                TP_TxControl.Process = TP_TxProcessIdle;                              // TP Process Selected To Idle State
                return;
            }
            if (TP_TxControl.ReceivedFC == 0x01) {                                    // Check if Flow Control Received
              if (TP_TxControl.FlowStatus == TP_FSOverflow) {                         // TP Flow Status Overflow Flag
                  UDS_Server.Status = UDS_ServerFree;                                 // UDS Server Status is Set To Free
                  TP_TxControl.Process = TP_TxProcessIdle;                            // TP Process Selected To Idle State
                  return;
              } else if (TP_TxControl.FlowStatus == TP_FSWait) {                      // TP Flow Status Wait
                  TP_Status.WaitCount++;                                              // TP Wait Counter Increamented
                  UDS_Server.Status = UDS_ServerWaiting;                              // UDS Server Status is Set To Waiting
                  if (TP_Status.WaitCount >= TP_ServerWaitCountDown) {                // Wait Counter Count Out Check
                      UDS_Server.Status = UDS_ServerFree;                             // UDS Server Status is Set To Free
                      TP_TxControl.Process = TP_TxProcessIdle;                        // TP Process Selected To Idle State
                      return;                                                         // TP Wait Counted Out
                  }
                  TP_TxControl.Process = TP_TxProcessWaiting;                         // TP Process Selected To Waiting State
                  TP_TxControl.Time = TP_Clock();                                     // TP Wait Entry Time Loaded
                  return;
              } else if (TP_TxControl.FlowStatus == TP_FSContinueToSend) {
                  if (TP_TxControl.BlocksAllowed == 0) {
                    TP_TxControl.FramesAllowed = TP_TxControl.TotalFrames             // TP Allowing Rest All Frames
                        - TP_TxControl.FrameCounter;
                  } else {
                    uint16_t BS = 0;
                    BS = TP_TxControl.TotalFrames - TP_TxControl.FrameCounter;        // TP Allowing Only BS Blocks Only
                    if (TP_TxControl.BlocksAllowed >= BS) {                           // If Blocks Allowed as More Than Data in Buffer
                      TP_TxControl.FramesAllowed = BS;                                // TP Allowing Rest All Frames
                    } else {
                      TP_TxControl.FramesAllowed = TP_TxControl.BlocksAllowed;        // TP Allowing Only Requested Frames
                    }
                  }
                  UDS_Server.Status = UDS_ServerTransmitting;                         // UDS Server Status is Set To Transmitting
                  TP_TxControl.Process = TP_TxProcessSeparationWait;                  // TP Process Selected To Frame Separation Wait State
                  TP_TxControl.Time = TP_Clock();                                     // TP Separation Time Entry Time Loaded
              }
              TP_TxControl.ReceivedFC = 0x00;
            }
            return;
        }

        case TP_TxProcessSFSending : {                                                // TP Process : Sending Single Frame State
            TP_TxFrameSF();                                                           // DoCAN Single Frame Transmission
            UDS_Server.Status = UDS_ServerFree;                                       // UDS Server Status is Set To Free
            TP_TxControl.Process = TP_TxProcessIdle;                                  // TP Process Selected To Idle State
            return;
        }

        default : {                                                                   // TP Process : Handling Rare Unexpected State
            TP_TxControl.Process = TP_TxProcessIdle;                                  // TP Process Selected To Idle State
            TP_Status.TxFlag = 0x00;                                                  // TP Reseting TX Flag
            UDS_Server.Status = UDS_ServerFree;                                       // UDS Server Status is Set To Free
            return;
        }

      }
    } else {                                                                          // Rare Exception Handling
      TP_Status.TxFlag = 0x00;                                                        // TP Reseting TX Flag
      return;
    }
}










void TP_RxDoCAN (void) {
    if (TP_Status.RxFlag == 0x00) {                                                   // Checking for RX Flag
      return;
    } else if (TP_Status.RxFlag == 0x01) {                                            // If CAN Message is Received
      TP_Status.RxFlag = 0;                                                           // RX Flag Reset
      if (TP_CheckCANID(TP_MessageRX.CANID.Raw, 'R')) {                               // Check CANID To Filter Junk Message
        return;                                                                       // Ignoring Junk Messages
      }
      uint8_t PCI = TP_MessageRX.Data[0];                                             // Reading PCI
      PCI = PCI >> 4;                                                                 // Extracting First 4 Bits

      switch (PCI) {                                                                  // Checking The Protocol Control Indicator
        // Receiving Single Frame
        case 0x00 : {                                                                 // Single Frame Type
          TP_RxFrameSF();                                                             // Single Frame Processing Function
          break;
        }
        // Receiving First Frame
        case 0x01 : {                                                                 // First Frame Type
          TP_RxFrameFF();                                                             // First Frame Processing Function
          break;
        }
        // Receiving Consecutive Frame
        case 0x02 : {                                                                 // Consecutive Frame Type
          TP_RxFrameCF();                                                             // Consecutive Frame Processing Function
          break;
        }
        // Receiving Flow Control Frame
        case 0x03 : {                                                                 // Flow Control Frame
          TP_RxFrameFC();                                                             // Flow Control Frame Processing Function
          break;
        }
        // Exception Handling
        default : {
          TP_SendNegativeResponse(UDS_NRC_IMLIF, TP_MessageRX.Data[1],                // NRC : Incorrect Format
                  TP_MessageRX.CANID.Raw);
          break;
        }
      }
    } else {
      TP_Status.RxFlag = 0;                                                           // Rare Exception Handling
      return;
    }
}





#endif