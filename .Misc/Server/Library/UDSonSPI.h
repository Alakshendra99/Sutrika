/* ==================================================================================================== */
/*
 *  Section
 *  Physical Layer
 *
 *  Physical Layer Implemented Using SPI as CAN
 *
 *  inline void SPI_Start (void)
 *  inline void SPI_ResetTimeout (void)
 *  inline void SPI_TransmitFrame (void)
 *  inline void SPI_TransmitFrameShow (void)
 *  inline void SPI_TransmitFrameBuild (uint16_t _CAN, uint8_t _D0, uint8_t _D1, uint8_t _D2, uint8_t _D3, 
          uint8_t _D4, uint8_t _D5, uint8_t _D6, uint8_t _D7)
 *  inline void SPI_ReceiveFrame (void)
 *  inline void SPI_ReceiveFrameShow (void)
 *  inline uint8_t SPI_ReceiveFrameBuild (void)
 */
 /* ---------------------------------------------------------------------------------------------------- */
#ifdef _UDSonSPI

#include "RachanaUDS.h"
#include "SansaadhanUDS.h"


typedef struct {
    Number16Bit CANID;
    uint8_t Counter;
    uint32_t Time;
    uint8_t Flag;
    uint8_t Data[8];
} SPI_MessageBufferRX;

typedef struct {
    Number16Bit CANID;
    uint8_t Data[8];
} SPI_MessageBufferTX;


SPI_MessageBufferTX SPI_FrameTX = {
    0x0000,
    {0x00},
};

SPI_MessageBufferRX SPI_FrameRX = {
    0x0000,
    0x00,
    0x00000000,
    0x00,
    {0x00},
};



inline void SPI_Start (void);   
inline void SPI_Start (void) {
    // SPI Setting
    SPR0(SPI_DIV128);                                                                 // SPI Clock Setting
    CPOL0(0);                                                                         // SPI Clock Polarity 0
    CPHA0(0);                                                                         // SPI Clock Phase 0
    DORD0(SPI_MSB);                                                                   // SPI Data Order MSB to LSB
    // SPI Slave Mode
    MSTR0(0);                                                                         // SPI Slave Mode 
    PB2Mode(0);                                                                       // SPI SS Input Mode
    PB2PullUP(1);                                                                     // SPI SS Pulled Up
    PB3Mode(0);                                                                       // SPI MOSI Input Mode
    PB3PullUP(1);                                                                     // SPI MOSI Pulled Up
    PB4Mode(1);                                                                       // SPI MISO Output Mode
    PB5Mode(1);                                                                       // SPI SCK Input Mode
    PB5PullUP(0);                                                                     // SPI SCK Pulled Dwon
    // SPI Start
    SPIE0(1);                                                                         // SPI Interrupt Enabled
    SPE0(1);                                                                          // SPI Peripheral Enabled
    PD2Mode(0);                                                                       // Reset Switch Mode Selected
    PD2PullUP(1);                                                                     // Reset Switch Pulled Up
}


inline void SPI_ResetTimeout (void);
inline void SPI_ResetTimeout (void) {
    if (SPI_FrameRX.Counter != 0) {                                                   // SPI Checking If Session Was Open
        if ((Tools.Clock() - SPI_FrameRX.Time ) > 1000) {                             // SPI Session Timeout Check
            SPI_FrameRX.Counter = 0;                                                  // SPI Counter Reset
            Tools.ConsolePrint("\n:(");                                               // SPI Session Expired Indication
        }
    }
}


inline void SPI_TransmitFrame (void);
inline void SPI_TransmitFrame (void) {
    // SPI Master Mode
    SPE0(0);                                                                          // SPI Peripheral Disabled
    SPIE0(0);                                                                         // SPI Interrupt Disabled
    MSTR0(1);                                                                         // SPI Master Mode 
    PB2Mode(1);                                                                       // SPI SS Output Mode
    PB2(1);                                                                           // SPI SS Idle High
    PB3Mode(1);                                                                       // SPI MOSI Output Mode
    PB4Mode(0);                                                                       // SPI MISO Input Mode
    PB4PullUP(1);                                                                     // SPI MISO Pulled Up
    PB5Mode(1);                                                                       // SPI SCK Output Mode
    SPE0(1);                                                                          // SPI Peripheral Enabled
    // SPI Data Sharing
    NOP(); NOP();                                                                     // SPI Delay
    PB2(0);                                                                           // SPI SS Driven Low
    NOP(); NOP();                                                                     // SPI Delay
      SPDR0('#');                                                                     // SPI Data Loaded
      NOP();                                                                          // SPI Delay
      while(!rSPIF0);                                                                 // SPI Waiting For Data Transfer
    NOP(); NOP();                                                                     // SPI Delay
      SPDR0(SPI_FrameTX.CANID.Byte.B1);                                               // SPI Data Loaded
      NOP();                                                                          // SPI Delay
      while(!rSPIF0);                                                                 // SPI Waiting For Data Transfer
    NOP(); NOP();                                                                     // SPI Delay
      SPDR0(SPI_FrameTX.CANID.Byte.B0);                                               // SPI Data Loaded
      NOP();                                                                          // SPI Delay
      while(!rSPIF0);                                                                 // SPI Waiting For Data Transfer
    uint8_t i = 0;
    while (i < 8) {
      NOP(); NOP();                                                                   // SPI Delay
      SPDR0(SPI_FrameTX.Data[i]);                                                     // SPI Data Loaded
      NOP();                                                                          // SPI Delay
      while(!rSPIF0);                                                                 // SPI Waiting For Data Transfer
      i++;
    }
    NOP(); NOP();                                                                     // SPI Delay
      SPDR0('#');                                                                     // SPI Data Loaded
      NOP();                                                                          // SPI Delay
      while(!rSPIF0);                                                                 // SPI Waiting For Data Transfer
    NOP(); NOP();                                                                     // SPI Delay
    PB2(1);                                                                           // SPI SS Driven High
    // SPI Slave Mode
    
    SPE0(0);                                                                          // SPI Peripheral Disabled
    i = rSPIF0;
    i = rSPDR0;
    MSTR0(0);                                                                         // SPI Slave Mode 
    PB2Mode(0);                                                                       // SPI SS Input Mode
    PB2PullUP(1);                                                                     // SPI SS Pulled Up
    PB3Mode(0);                                                                       // SPI MOSI Input Mode
    PB3PullUP(1);                                                                     // SPI MOSI Pulled Up
    PB4Mode(1);                                                                       // SPI MISO Output Mode
    PB5Mode(1);                                                                       // SPI SCK Input Mode
    PB5PullUP(0);                                                                     // SPI SCK Pulled Dwon
    SPIE0(1);                                                                         // SPI Interrupt Enabled
    SPE0(1);                                                                          // SPI Peripheral Enabled
}


inline void SPI_TransmitFrameShow (void);
inline void SPI_TransmitFrameShow (void) {
    Tools.ConsoleLine();                                                              // SPI Show Line Break
    Tools.ConsolePrint("Tx: 0x");                                                     // SPI Show Header
    Tools.ConsoleHEX(SPI_FrameTX.CANID.Raw);                                          // SPI Show CAN ID
    Tools.ConsolePrint(" :");                                                         // SPI Show Data Separater
    uint8_t i = 0;
    while (i < 8) {
      Tools.ConsolePrint(' ');                                                        // SPI Show Data Space
      Tools.ConsoleHEX(SPI_FrameTX.Data[i]);                                          // SPI Show Data
      i++;
    }
}


inline void SPI_TransmitFrameBuild (uint16_t _CAN, uint8_t _D0, uint8_t _D1, uint8_t _D2, uint8_t _D3, 
  uint8_t _D4, uint8_t _D5, uint8_t _D6, uint8_t _D7);
inline void SPI_TransmitFrameBuild (uint16_t _CAN, uint8_t _D0, uint8_t _D1, uint8_t _D2, uint8_t _D3, 
  uint8_t _D4, uint8_t _D5, uint8_t _D6, uint8_t _D7) {
    SPI_FrameTX.CANID.Raw = _CAN;                                                     // SPI TX Frame CAN ID
    SPI_FrameTX.Data[0] = _D0;                                                        // SPI TX Frame Data Byte 0
    SPI_FrameTX.Data[1] = _D1;                                                        // SPI TX Frame Data Byte 1
    SPI_FrameTX.Data[2] = _D2;                                                        // SPI TX Frame Data Byte 2
    SPI_FrameTX.Data[3] = _D3;                                                        // SPI TX Frame Data Byte 3
    SPI_FrameTX.Data[4] = _D4;                                                        // SPI TX Frame Data Byte 4
    SPI_FrameTX.Data[5] = _D5;                                                        // SPI TX Frame Data Byte 5
    SPI_FrameTX.Data[6] = _D6;                                                        // SPI TX Frame Data Byte 6
    SPI_FrameTX.Data[7] = _D7;                                                        // SPI TX Frame Data Byte 7
}


inline void SPI_ReceiveFrame (void);
inline void SPI_ReceiveFrame (void) {
    uint8_t Data = 0;
    switch (SPI_FrameRX.Counter) {                                                    // SPI Checking SPI Counter
        case 0 : {
            Data = rSPDR0;                                                            // SPI Data Read
            SPI_FrameRX.Time = Tools.Clock();                                         // SPI Timer Expire
            SPI_FrameRX.Flag = ((Data == '#') ? 0x01 : 0x00);                         // SPI Flag Set
            SPI_FrameRX.Counter++;                                                    // SPI Counter Increment
            return;
        } 
        case 1 : {
            Data = rSPDR0;                                                            // SPI Data Read
            SPI_FrameRX.CANID.Byte.B1 = Data;                                         // SPI CAN ID High Byte Loaded
            SPI_FrameRX.Counter++;                                                    // SPI Counter Increment
            return;
        }
        case 2 : {
            Data = rSPDR0;                                                            // SPI Data Read
            SPI_FrameRX.CANID.Byte.B0 = Data;                                         // SPI CAN ID Low Byte Loaded
            SPI_FrameRX.Counter++;                                                    // SPI Counter Increment
            return;
        }
        case 3 : {
            Data = rSPDR0;                                                            // SPI Data Read
            SPI_FrameRX.Data[0] = Data;                                               // SPI Data Loaded
            SPI_FrameRX.Counter++;                                                    // SPI Counter Increment
            return;
        }
        case 4 : {
            Data = rSPDR0;                                                            // SPI Data Read
            SPI_FrameRX.Data[1] = Data;                                               // SPI Data Loaded
            SPI_FrameRX.Counter++;                                                    // SPI Counter Increment
            return;
        }
        case 5 : {
            Data = rSPDR0;                                                            // SPI Data Read
            SPI_FrameRX.Data[2] = Data;                                               // SPI Data Loaded
            SPI_FrameRX.Counter++;                                                    // SPI Counter Increment
            return;
        }
        case 6 : {
            Data = rSPDR0;                                                            // SPI Data Read
            SPI_FrameRX.Data[3] = Data;                                               // SPI Data Loaded
            SPI_FrameRX.Counter++;                                                    // SPI Counter Increment
            return;
        }
        case 7 : {
            Data = rSPDR0;                                                            // SPI Data Read
            SPI_FrameRX.Data[4] = Data;                                               // SPI Data Loaded
            SPI_FrameRX.Counter++;                                                    // SPI Counter Increment
            return;
        }
        case 8 : {
            Data = rSPDR0;                                                            // SPI Data Read
            SPI_FrameRX.Data[5] = Data;                                               // SPI Data Loaded
            SPI_FrameRX.Counter++;                                                    // SPI Counter Increment
            return;
        }
        case 9 : {
            Data = rSPDR0;                                                            // SPI Data Read
            SPI_FrameRX.Data[6] = Data;                                               // SPI Data Loaded
            SPI_FrameRX.Counter++;                                                    // SPI Counter Increment
            return;
        }
        case 10 : {
            Data = rSPDR0;                                                            // SPI Data Read
            SPI_FrameRX.Data[7] = Data;                                               // SPI Data Loaded
            SPI_FrameRX.Counter++;                                                    // SPI Counter Increment
            return;
        }
        case 11 : {
            Data = rSPDR0;                                                            // SPI Data Read
            SPI_FrameRX.Flag = ((Data == '#') ? (SPI_FrameRX.Flag + 1) : 0x00);       // SPI Flag Acknowledgment
            SPI_FrameRX.Counter++;                                                    // SPI Counter Increment
            return;
        }
        default : {
            SPI_FrameRX.Counter = 0;                                                  // SPI Counter Reset
            return;
        }
    }
}


inline void SPI_ReceiveFrameShow (void);
inline void SPI_ReceiveFrameShow (void) {
    Tools.ConsoleLine();                                                              // SPI Show Line Break
    Tools.ConsolePrint("Rx: 0x");                                                     // SPI Show Header
    Tools.ConsoleHEX(SPI_FrameRX.CANID.Raw);                                          // SPI Show CAN ID
    Tools.ConsolePrint(" :");                                                         // SPI Show Data Separater
    uint8_t i = 0;
    while (i < 8) {
      Tools.ConsolePrint(' ');                                                        // SPI Show Data Space
      Tools.ConsoleHEX(SPI_FrameRX.Data[i]);                                          // SPI Show Data
      i++;
    }
}


inline uint8_t SPI_ReceiveFrameBuild (void);
inline uint8_t SPI_ReceiveFrameBuild (void) {
    if (SPI_FrameRX.Flag == 0x02) {                                                   // SPI Check If All OK
        SPI_FrameRX.Flag = 0x00;                                                      // SPI Flag Reset
        SPI_FrameRX.Counter = 0;                                                      // SPI Counter Reset
        SPI_ReceiveFrameShow();                                                       // SPI receive Frame Show
        TP_ReceiveDataCAN(SPI_FrameRX.CANID.Raw, SPI_FrameRX.Data[0],                 // SPI To TP Transfer
            SPI_FrameRX.Data[1], SPI_FrameRX.Data[2], SPI_FrameRX.Data[3], 
            SPI_FrameRX.Data[4], SPI_FrameRX.Data[5], SPI_FrameRX.Data[6],
            SPI_FrameRX.Data[7]);
        return 1;                                                                     // SPI Received A Frame
    }
    return 0;                                                                         // SPI Not Received Any Frame
}

Interrupt(INT_SPI0_STC) {
    SPI_ReceiveFrame();                                                               // SPI ReceiveFrame Interrupt Function
}

inline void SPI_Reset (void);
inline void SPI_Reset (void) {
    if (rPD2 == 0) {
      Tools.Signal(1);
      Tools.ConsolePrint("\n\n\nResetting . . .\n\n\n");
      Tools.Delay(1000);
      Tools.Signal(0);
      Reset();
    }
}
#endif
/* ==================================================================================================== */