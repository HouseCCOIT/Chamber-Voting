//-----------------------------------------------------------------------------
//
// O22STRCT.h
// Opto 22 OptoMMP System Developer Kit (SDK) PAC-DEV-OPTOMMP-CPLUS
// Copyright (c) 1999 - 2018 by Opto 22
//
// This is a support file for "O22SIOMM.H" and its class, O22SnapIoMemMap.
//
// Most of the structures in this file map directly or closely to areas
// of the I/O unit's memory map.
//-----------------------------------------------------------------------------


#ifndef __O22STRCT_H
#define __O22STRCT_H

/** Digital Bank read-only area.
 *
 * Represents the current state of all standard density digital points.
 *
 * See Appendix A - Digital Bank Read - Read Only in the OptoMMP Protocol Guide for additional details.
 */
typedef struct SIOMM_DigBankReadArea
{
    int nStatePts63to32;          ///< Current state of points 32 through 63 as a bitmask
    int nStatePts31to0;           ///< Current state of points 0 through 31 as a bitmask
    int nOnLatchStatePts63to32;   ///< Current on-latch state of points 32 through 63 as a bitmask
    int nOnLatchStatePts31to0;    ///< Current on-latch state of points 0 through 31 as a bitmask
    int nOffLatchStatePts63to32;  ///< Current off-latch state of points 32 through 63 as a bitmask
    int nOffLatchStatePts31to0;   ///< Current off-latch state of points 0 through 31 as a bitmask
    int nActiveCountersPts63to32; ///< Current counter activation state (e.g. on/off) of points 32 through 63 as a bitmask
    int nActiveCountersPts31to0;  ///< Current counter activation state (e.g. on/off) of points 0 through 31 as a bitmask
} O22_SIOMM_DigBankReadArea;

/** Digital Point read-only area
 *
 * Current state of a single digital point.
 *
 * See Appendix A - Digital Point Read - Read Only in the OptoMMP Protocol Guide for additional details.
 */
typedef struct SIOMM_DigPointReadArea
{
    int  nState;          ///< Boolean state of the digital point
    int  nOnLatch;        ///< Boolean state of the point's on latch
    int  nOffLatch;       ///< Boolean state of the point's off latch
    int  nCounterState;   ///< Whether or not the point's counter is active
    int  nCounts;         ///< Current counter value of the point.
} O22_SIOMM_DigPointReadArea;


/** Analog Point read-only area
 *
 * Current state of a single analog point.
 *
 * See Appendix A - (Old) Analog Point Read - Read Only in the OptoMMP Protocol Guide for additional details.
 */
typedef struct SIOMM_AnaPointReadArea
{
    float fValue;         ///< Current floating point value (in engineering units) of the analog point.
    float fCounts;        ///< Current counter value of the point.
    float fMinValue;      ///< Lowest value recorded by the point since the last time it was cleared.
    float fMaxValue;      ///< Highest value recorded by the point since the last time it was cleared.
    short nRawCounts;
} O22_SIOMM_AnaPointReadArea;


/** Generic array of floating point values, used in SIOMM_AnaBanks
 * @see SIOMM_AnaBanks
 */
typedef struct SIOMM_AnaBank
{
    float fValue[64];     ///< Generic array of floating point values, used in SIOMM_AnaBanks
} O22_SIOMM_AnaBank;

/** Analog point bank read-only area
 * See Appendix A - Analog Bank Read - Read Only in the OptoMMP Protocol Guide for additional details.
 */
typedef struct SIOMM_AnaBanks
{
    struct SIOMM_AnaBank Values;    ///< Current values (in engineering units) of all analog points.
    struct SIOMM_AnaBank Counts;    ///< Current counts of all analog points.
    struct SIOMM_AnaBank MinValues; ///< Minimum recorded values of all analog points.
    struct SIOMM_AnaBank MaxValues; ///< Maximum recorded values of all analog points.
} O22_SIOMM_AnaBanks;


/** Point configuration read/write area.
 *
 * This is the point configuration structure used with I/O units running firmware 8.0 or later.
 *
 * See Appendix A - (Expanded) Analog and Digital Point Configuration Information - Read/Write
 * in the OptoMMP Protocol Guide for additional details.
 */
typedef struct SIOMM_PointConfigArea4
{
    int  nModuleType;          ///< Read only.
    int  nPointType;           ///< Read/Write
    int  nFeature;             ///< Read/Write.  Only used for digital points
    float fOffset;             ///< Read/Write.  Only used for analog points
    float fGain;               ///< Read/Write.  Only used for analog points
    float fHiScale;            ///< Read/Write.  Only used for analog points
    float fLoScale;            ///< Read/Write.  Only used for analog points
    float fFilterWeight;       ///< Read/Write.  Only used for analog points
    float fWatchdogValue;      ///< Read/Write
    int  nWatchdogEnabled;     ///< Read/Write
    unsigned char byName[51];  ///< Read/Write
} O22_SIOMM_PointConfigArea4;

// TODO: SIOMM4096_PointConfigArea2 originally appeared as its own definition, but was
// identical to SIOMM_PointConfigArea4. I've collapsed it down to a simple typedef, but
// that might break something in the ActiveX building stuff. Need to test it.
typedef struct SIOMM_PointConfigArea4 SIOMM4096_PointConfigArea2;

/** Point configuration read/write area.
 *
 * This point configuration structure is used with I/O units running firmware older than 8.0.
 *
 * See Appendix A - (Old) Analog and Digital Point Configuration Information - Read/Write
 * in the OptoMMP Protocol Guide for additional details.
 */
typedef struct SIOMM_PointConfigArea3
{
    int  nModuleType;          ///< Read only.
    int  nPointType;           ///< Used to specify a point type, used for digital points.
    int  nFeature;             ///< Set point features. Only used for digital points.
    float fOffset;             ///< Only used for analog points.
    float fGain;               ///< Only used for analog points.
    float fHiScale;            ///< Only used for analog points.
    float fLoScale;            ///< Only used for analog points.
    float fFilterWeight;       ///< Only used for analog points.
    float fWatchdogValue;      ///< Only used for analog points.
    int  nWatchdogEnabled;     ///< Only used for analog points.
    unsigned char byName[16];  ///< Only used for analog points.
    float fHiClamp;            ///< Only used for analog points.
    float fLoClamp;            ///< Only used for analog points.
} O22_SIOMM_PointConfigArea3;


/** Point configuration read/write area.
 *
 * This point configuration structure is used with I/O units running firmware older than 8.0.
 *
 * See Appendix A - (Old) Analog and Digital Point Configuration Information - Read/Write
 * in the OptoMMP Protocol Guide for additional details.
 */
typedef struct SIOMM_PointConfigArea2
{
    int  nModuleType;          ///> Read only.
    int  nPointType;           ///> Read/Write
    int  nFeature;             ///> Read/Write.  Only used for digital points
    float fOffset;             ///> Read/Write.  Only used for analog points
    float fGain;               ///> Read/Write.  Only used for analog points
    float fHiScale;            ///> Read/Write.  Only used for analog points
    float fLoScale;            ///> Read/Write.  Only used for analog points
    float fFilterWeight;       ///> Read/Write.  Only used for analog points
    float fWatchdogValue;      ///> Read/Write
    int  nWatchdogEnabled;     ///> Read/Write
    unsigned char byName[16];  ///> Read/Write
} O22_SIOMM_PointConfigArea2;

/** Point configuration read/write area.
 *
 * This point configuration structure is used with I/O units running firmware older than 8.0.
 *
 * See Appendix A - (Old) Analog and Digital Point Configuration Information - Read/Write
 * in the OptoMMP Protocol Guide for additional details.
 */
typedef struct SIOMM_PointConfigArea
{
    int  nModuleType;       ///> Read only.
    int  nPointType;        ///> Read/Write
    int  nFeature;          ///> Read/Write.  Only used for digital points
    float fOffset;          ///> Read/Write.  Only used for analog points
    float fGain;            ///> Read/Write.  Only used for analog points
    float fHiScale;         ///> Read/Write.  Only used for analog points
    float fLoScale;         ///> Read/Write.  Only used for analog points
    float fWatchdogValue;   ///> Read/Write
    int  nWatchdogEnabled;  ///> Read/Write
} O22_SIOMM_PointConfigArea;

typedef struct SIOMM_PidLoopConfigArea
{
    float  fGain;          ///< Tuning constant, Proportional
    float  fTuneI;         ///< Tuning constant, Integral. Reset rate in units of 1/min
    float  fTuneD;         ///< Tuning constant, Derivative
    float  fTuneFF;        ///< Tuning constant, Feed Forward
    float  fFeedForward;   ///< Bias or FeedForward.

    float  fInRangeLo;     ///< Lower bound on the valid range for input
    float  fInRangeHi;     ///< Upper bound on the valid range for input
    float  fOutClampLo;    ///< Lower bound to clamp output to
    float  fOutClampHi;    ///< Upper bound to clamp output to
    float  fOutMaxChange;  ///< Maximum change for the output
    float  fOutMinChange;  ///< Minimum change for the output

    float  fScanSec;       ///< PID scan time in seconds. Minimum value is 0.001 (1 millisecond)

    float  fOutWhenInLo;   ///< The output value when the input is low
    float  fOutWhenInHi;   ///< The output value when the input is high

    int   nInputMemMap;    ///< The memory map address of the input for this PID loop for cascading PIDs, or 0 if input will be written directly
    int   nSetpointMemMap; ///< The memory map address of the setpoint for this PID loop for cascading PIDs, or 0 if the setpoint will be written directly
    int   nOutputMemMap;   ///< The memory map address to write the output of this PID loop for cascading PIDs, or 0 if no output will be written
    int   nAlgorithm;      ///< Which algorithm to use (see Appendix A - PID Configuration and Status - Read/Write of the OptoMMP Protocol Guide for available algorithms) See KB82058 at www.opto22.com. Firmware 9.4c and later have updated algorithms.
    int   nManual;         ///< Manual mode. 1 = Yes, 0 = No
    int   nCfgFlags;       ///< Configuration flags(see Appendix A - PID Configuration and Status - Read/Write of the OptoMMP Protocol Guide for available flags)
} O22_SIOMM_PidLoopConfigArea;

typedef struct SIOMM_TpoConfigArea
{
    float fPercent;
    float fPeriod;
    float fDelaySec;
    int  nPulseQty;
    int  nCfgWord;
    int  nOutputMemMap;
    int  nOutputMask;
    int  nOutputMemMapOff;
} O22_SIOMM_TpoConfigArea;

typedef struct SIOMM_MOMO
{
    int nOn63to32;
    int nOn31to0;
    int nOff63to32;
    int nOff31to0;
} O22_SIOMM_MOMO;

typedef struct SIOMM_EventMsgConfigArea
{
    int           nState;            ///< Current message state: Inactive/Active/Acknowledged
    int           nScratchOn63to32;  ///< Bitmask showing which scratchpad bits are turned on (bits 32-63)
    int           nScratchOn31to0;   ///< Bitmask showing which scratchpad bits are turned on (bits 0-31)
    int           nScratchOff63to32; ///< Bitmask showing which scratchpad bits are turned off (bits 32-63)
    int           nScratchOff31to0;  ///< Bitmask showing which scratchpad bits are turned on (bits 0-31)
    int           nStreamEnabled;    ///< Enable streaming. (Boolean)
    int           nStreamPeriod;     ///< Stream period in seconds. 0 = send once, maximum = 604800
    int           nEmailEnabled;     ///< Enable email. (Boolean).
    int           nEmailPeriod;      ///< Email period in seconds. 0 = send once, maximum = 604800
    int           nSnmpEnabled;      ///< Email SNMP trap. (Boolean).
    int           nSnmpStreamPeriod; ///< SNMP trap period in seconds. 0 = send once, maximum = 604800
    int           nSnmpSpecificType; ///< SNMP trap type
    int           nPriority;         ///< Message priority. 0 = high, 1 = low. Applies only if you are using SNMP with a modem connection, and the I/O unit is dialing out.
    // 1 DWORD reserved
    int           nSerialEnabled;    ///< Enable serial module message. (Boolean)
    int           nSerialStreamPeriod; ///< Serial stream period in seconds.  0 = send once, maximum = 604800
    unsigned char  byMessage[128];   ///< Message text, limited to 127 characters. Used with email, serial and SNMP messages.
} O22_SIOMM_EventMsgConfigArea;


typedef struct SIOMM_DigitalEventConfigArea
{
    int           nEventDigitalOn63to32;     ///< Digital point states to watch as a bitmask (points 32-63)
    int           nEventDigitalOn31to0;      ///< Digital point states to watch as a bitmask (points 0-31)
    int           nEventDigitalOff63to32;    ///< Digital point states to watch as a bitmask (points 32-63)
    int           nEventDigitalOff31to0;     ///< Digital point states to watch as a bitmask (points 0-31)
    int           nEventScratchOn63to32;     ///< Scratchpad on bits to watch (bits 32-63)
    int           nEventScratchOn31to0;      ///< Scratchpad on bits to watch (bits 0-31)
    int           nEventScratchOff63to32;    ///< Scratchpad off bits to watch (bits 32-63)
    int           nEventScratchOff31to0;     ///< Scratchpad off bits to watch (bits 0-31)

    int           nReactionDigitalOn63to32;  ///< Digital points to turn on in reaction to triggered evens (points 32-63)
    int           nReactionDigitalOn31to0;   ///< Digital points to turn on in reaction to triggered event (points 0-31)
    int           nReactionDigitalOff63to32; ///< Digital points to turn off in reaction to triggered event (points 32-63)
    int           nReactionDigitalOff31to0;  ///< Digital points to turn off in reaction to triggered event (points 0-31)
    int           nReactionScratchOn63to32;  ///< Scratchpad bits to turn on in reaction to triggered event (bits 32-63)
    int           nReactionScratchOn31to0;   ///< Scratchpad bits to turn on in reaction to triggered event (bits 0-31)
    int           nReactionScratchOff63to32; ///< Scratchpad bits to turn off in reaction to triggered event (bits 32-63)
    int           nReactionScratchOff31to0;  ///< Scratchpad bits to turn on in reaction to triggered event (bits 0-31)
} O22_SIOMM_DigitalEventConfigArea;



typedef struct SIOMM_AlarmEventConfigArea
{
    int  nDeviationInAlarm;          ///< Deviation alarm state: non-zero if the alarm is currently triggered
    int  nDeviationEnabled;          ///< Enable Deviation alarm. (Boolean)
    float fDeviationMiddleRange;     ///< Deviation alarm, previous deviation value
    float fDeviationAmount;          ///< Deviation alarm, current deviation amount
    int  nDeviationScratchOn63to32;  ///< Scratchpad bits to turn on in reaction to a deviation alarm (bits 32-63)
    int  nDeviationScratchOn31to0;   ///< Scratchpad bits to turn on in reaction to a deviation alarm (bits 0-31)
    int  nDeviationScratchOff63to32; ///< Scratchpad bits to turn off in reaction to a deviation alarm (bits 32-63)
    int  nDeviationScratchOff31to0;  ///< Scratchpad bits to turn off in reaction to a deviation alarm (bits 0-31)
    int  nHiAlarmInAlarm;            ///< High alarm state: non-zero if the alarm is currently triggered
    int  nHiAlarmEnabled;            ///< Enable high alarm. (Boolean)
    float fHiAlarmLimit;             ///< High alarm setpoint: values higher than this will trigger the alarm
    float fHiAlarmLimitDeadband;
    int  nHiAlarmScratchOn63to32;    ///< Scratchpad bits to turn on in reaction to a high alarm (bits 32-63)
    int  nHiAlarmScratchOn31to0;     ///< Scratchpad bits to turn on in reaction to a high alarm (bits 0-31)
    int  nHiAlarmScratchOff63to32;   ///< Scratchpad bits to turn off in reaction to a high alarm (bits 32-63)
    int  nHiAlarmScratchOff31to0;    ///< Scratchpad bits to turn off in reaction to a high alarm (bits 0-31)
    int  nLoAlarmInAlarm;            ///< Low alarm state: non-zero if the alarm is currently triggered
    int  nLoAlarmEnabled;            ///< Enable low alarm. (Boolean)
    float fLoAlarmLimit;             ///< Low alarm setpoint: values lower than this will trigger the alarm
    float fLoAlarmLimitDeadband;
    int  nLoAlarmScratchOn63to32;    ///< Scratchpad bits to turn on in reaction to a low alarm (bits 32-63)
    int  nLoAlarmScratchOn31to0;     ///< Scratchpad bits to turn on in reaction to a low alarm (bits 0-31)
    int  nLoAlarmScratchOff63to32;   ///< Scratchpad bits to turn off in reaction to a high alarm (bits 32-63)
    int  nLoAlarmScratchOff31to0;    ///< Scratchpad bits to turn off in reaction to a high alarm (bits 0-31)
    int  nValueLogicalAdr;           ///< Memory map address of the value to watch
    int  nIsFloat;                   ///< Is value a float? 1 for yes, 0 for no.
    // BYTE            Pad[0x18];
} O22_SIOMM_AlarmEventConfigArea;

typedef struct SIOMM_SerialEventConfigArea
{
    int          nSerialPortsMask;  ///< Serial port mask. Bits 0-31 correspond to ports 0-31. On bits represent serial ports to monitor for the event string. Event occurs if any of the ports receives the event string.
    // int nUnused;                 // expansion space for bit mask, in case we ever support more ports (up to 64)
    int          nTrapType;         ///< SNMP trap type. Only used if sending an SNMP trap as a reaction to the serial event.
    int          nTrapPeriod;       ///< SNMP trap period in seconds. How often to send a trap as a reaction to the serial event.
    unsigned char byPattern[40];    ///< Pattern string for the event. Wildcards (* and ?) are allowed.
    unsigned char byReplace[40];    ///< Reaction string to be sent with the SNMP trap.
    int          nScratchOn63to32;  ///< Scratchpad bits to turn on in reaction to the event (bits 32-63)
    int          nScratchOn31to0;   ///< Scratchpad bits to turn on in reaction to the event (bits 0-31)
    int          nScratchOff63to32; ///< Scratchpad bits to turn off in reaction to the event (bits 32-63)
    int          nScratchOff31to0;  ///< Scratchpad bits to turn off in reaction to the event (bits 0-31)
    int          nSendEmail;        ///< Enable sending an email message in response to the event. Boolean.
} O22_SIOMM_SerialEventConfigArea;


typedef struct SIOMM_TimerEventConfigArea
{
    int nDigitalIn_On63to32;        ///< Mask of digital points that will start the timer when they're set ON, points 32-63
    int nDigitalIn_On31to0;         ///< Mask of digital points that will start the timer when they're set ON, points 0-31
    int nDigitalIn_Off63to32;       ///< Mask of digital points that will start the timer when they're set OFF, points 32-63
    int nDigitalIn_Off31to0;        ///< Mask of digital points that will start the timer when they're set OFF, points 0-31
    int nScratchIn_On63to32;        ///< Mask of scratch pad bits that will start the timer when they're set ON, bits 32-63
    int nScratchIn_On31to0;         ///< Mask of scratch pad bits that will start the timer when they're set ON, bits 0-31
    int nScratchIn_Off63to32;       ///< Mask of scratch pad bits that will start the timer when they're set OFF, bits 32-63
    int nScratchIn_Off31to0;        ///< Mask of scratch pad bits that will start the timer when they're set OFF, bits 0-31
    int nDigitalOut_On63to32;       ///< Mask of digital points to be set ON when the timer expires, points 32-63
    int nDigitalOut_On31to0;        ///< Mask of digital points to be set ON when the timer expires, points 0-31
    int nDigitalOut_Off63to32;      ///< Mask of digital points to be set OFF when the timer expires, points 32-63
    int nDigitalOut_Off31to0;       ///< Mask of digital points to be set OFF when the timer expires, points 0-31
    int nScratchOut_On63to32;       ///< Mask of scratchpad bits to be set ON when the timer expires, bits 32-63
    int nScratchOut_On31to0;        ///< Mask of scratchpad bits to be set ON when the timer expires, bits 0-31
    int nScratchOut_Off63to32;      ///< Mask of scratchpad bits to be set OFF when the timer expires, bits 32-63
    int nScratchOut_Off31to0;       ///< Mask of scratchpad bits to be set OFF when the timer expires, bits 0-31
    int nPresetMsec;                ///< Length of the timer delay, in milliseconds
    // int reserved;
    int nRemainingMsec;             ///< Time remaining until the timer expires, in milliseconds
    int nState;                     ///< 0=off, 1=on, 2=Timer is ticking
} O22_SIOMM_TimerEventConfigArea;


typedef struct SIOMM_StatusVersion
{
    int          nMapVer;            //  Memory map version
    int          nLoaderVersion;     //  Loader version (1.2.3.4 format)
    int          nKernelVersion;     //  Kernel version (1.2.3.4 format)
} O22_SIOMM_StatusVersion;


typedef struct SIOMM_StatusHardware2
{
    int          nIoUnitType;        //  I/O unit type
    unsigned char byHwdVerMonth;      //  hardware version (month)
    unsigned char byHwdVerDay;        //  hardware version (day)
    short         wHwdVerYear;        //  hardware version (4 digit year)
    int          nRamSize;           //  bytes of installed RAM
    unsigned char byPartNumber[32];   //  part number as a string
} O22_SIOMM_StatusHardware2;

typedef struct SIOMM_StatusHardware
{
    int          nIoUnitType;        //  I/O unit type
    unsigned char byHwdVerMonth;      //  hardware version (month)
    unsigned char byHwdVerDay;        //  hardware version (day)
    short         wHwdVerYear;        //  hardware version (4 digit year)
    int          nRamSize;           //  bytes of installed RAM
} O22_SIOMM_StatusHardware;


typedef struct SIOMM_StatusNetwork2
{
    short         wMACAddress0;       //  MAC address
    short         wMACAddress1;       //  MAC address
    short         wMACAddress2;       //  MAC address
    int          nTCPIPAddress;      //  IP address
    int          nSubnetMask;        //  subnet mask
    int          nDefGateway;        //  default gateway
    int          nTcpIpMinRtoMS;     //  TCP/IP minimum Response Timeout (RTO)
    //    in milliseconds
    int          nInitialRtoMS;      //  initial RTO
    int          nTcpRetries;        //  number of TCP retries
    int          nTcpIdleTimeout;    //  TCP idle session timeout
    int          nEnetLateCol;       //  Ethernet late collisions
    int          nEnetExcessiveCol;  //  Ethernet excessive collisions
    int          nEnetOtherErrors;   //  Other Ethernet errors
} O22_SIOMM_StatusNetwork2;

typedef struct SIOMM_StatusNetwork
{
    short         wMACAddress0;       //  MAC address
    short         wMACAddress1;       //  MAC address
    short         wMACAddress2;       //  MAC address
    int          nTCPIPAddress;      //  IP address
    int          nSubnetMask;        //  subnet mask
    int          nDefGateway;        //  default gateway
} O22_SIOMM_StatusNetwork;

typedef struct SIOMM_StatusWrite
{
    int          nBootpAlways;         //
    int          nDegreesFC;           //
    int          nReserved1;           //
    int          nWatchdogTimeMS;      //
    int          nTcpIpMinRto;         //
    int          nTcpIpInitRto;        //
    int          nTcpIpRetries;        //
    int          nTcpIpIdleTimeoutMS;  //
    int          nReserved2[10];       //
    int          nDigMaxScanMS;        //
    int          nAnaMaxScanMS;        //
    //    int          byReserved3[64];      //
} O22_SIOMM_StatusWrite;

typedef struct SIOMM_SerialModuleConfigArea
{
    int          nIpPort;            //  READ ONLY
    int          nBaudRate;          //  baud rate
    unsigned char byParity;           //  parity
    unsigned char byDataBits;         //  data bits (7 or 8)
    unsigned char byStopBits;         //  stop bits (1 or 2)
    unsigned char byTestMessage;      //  bool for sending a powerup test msg
    unsigned char byEOM1;             //  first  end-of-message character
    unsigned char byEOM2;             //  second end-of-message character
    unsigned char byEOM3;             //  third  end-of-message character
    unsigned char byEOM4;             //  fourth end-of-message character
} O22_SIOMM_SerialModuleConfigArea;


typedef struct SIOMM_StreamCustomBlock
// Be careful when making changes to this structure!
{
    // The first three variables map directly to a custom UDP stream packet.
    int           nHeader;        // See below for info
    int           nMemMapAddress; // Memory address of custom stream area.
    unsigned char  byData[2034];   // Max data size of 2034
    int           nTCPIPAddress;  // The source IP address
} O22_SIOMM_StreamCustomBlock;

typedef struct SIOMM_StreamStandardBlock
{
    int           nHeader;                 ///< Packet header. See Chapter 2 - Streaming Data in the OptoMMP Protocol Guide for details

    // The following items map directly to the standard stream data
    float         fAnalogValue[64];         ///< Analog engineering units data for 64 points.
    int           nDigPointFeature[64];     ///< Digital point feature data for 64 points. Does not include high density digital points.
    int           nStatePts63to32;          ///< On/off state for digital points 32-63. Does not include high density digital points.
    int           nStatePts31to0;           ///< On/off state for digital points 0-31. Does not include high density digital points.
    int           nOnLatchStatePts63to32;   ///< On-latch state for digital points 32-63. Does not include high density digital points.
    int           nOnLatchStatePts31to0;    ///< On-latch state for digital points 0-31 Does not include high density digital points.
    int           nOffLatchStatePts63to32;  ///< Off-latch state for digital points 32-63. Does not include high density digital points.
    int           nOffLatchStatePts31to0;   ///< Off-latch state for digital points 0-31. Does not include high density digital points.
    int           nActiveCountersPts63to32; ///< On/off state of counters for digital points 32-63. Does not include high density digital points.
    int           nActiveCountersPts31to0;  ///< On/off state of counters for digital points 0-31. Does not include high density digital points.
    unsigned char byReserved[56];           ///< reserved for future use

    int           nTCPIPAddress;            ///< The source IP address.

    // Be careful when making changes to this structure!

    /*
     short          wMonth;
     short          wDay;
     short          wYear;
     short          wHour;
     short          wMinute;
     short          wSecond;
     short          wMillisecond;
     */
} O22_SIOMM_StreamStandardBlock;


/////////////////////////////////////////////////////////////////////
// Scratch Pad area
//

typedef struct SIOMM_ScratchPadIntegerBlock
{
    int nValue[256];
} O22_SIOMM_ScratchPadIntegerBlock;

typedef struct SIOMM_ScratchPadFloatBlock
{
    float fValue[256];
} O22_SIOMM_ScratchPadFloatBlock;

typedef struct SIOMM_ScratchPadString
{
    short         wLength;        ///< Length of the string in byString
    unsigned char byString[128];  ///< String data
} O22_SIOMM_ScratchPadString;

typedef struct SIOMM_ScratchPadInteger64Block
{
    int nValue[2048];
} O22_SIOMM_ScratchPadInteger64Block;

// The following structures are for the ActiveX component
#ifdef __midl
typedef struct SIOMM_ScratchPadStringX
{
    short         wLength;
    unsigned char byString[128];
    BSTR          bstrString;
} O22_SIOMM_ScratchPadStringX;

typedef struct SIOMM_ScratchPadStringBlock
{
    O22_SIOMM_ScratchPadStringX String[8];
} O22_SIOMM_ScratchPadStringBlock;
#endif

typedef struct SIOMM_RTC_DATE_TIME
{
    short          wYear;       ///< Year (e.g. 2013)
    unsigned char  byMonth;     ///< Month
    unsigned char  byDay;       ///< Day of the month
    unsigned char  byHours;     ///< Hour in a 24 hour day
    unsigned char  byMinutes;   ///< Minutes
    unsigned char  bySeconds;   ///< Seconds
    unsigned char  byHSeconds;  ///< 100th of a second
} O22_SIOMM_RTC_DATE_TIME;


typedef struct SIOMM_DataLogSample
{
    struct SIOMM_RTC_DATE_TIME  Time; ///< Time of the data sample
    int                 nMemMap;      ///< Memory map address the data comes from
    int                 nFormat;      ///< Data format (66 for float, 64 for signed integer, 78 for unsigned integer)

    union
    {
        int       nInteger;           ///< Sample value as an integer
        float      fFloat;            ///< Sample value as a float
        int       nBitmask;           ///< Sample value as a bitmask
    } unData;

} O22_SIOMM_DataLogSample;

/////////////////////////////////////////////////////////////////////
// EPIC Module Quality 0xffff f100 2000
//
typedef struct
{
	unsigned int uModules63To32Bitmask;
	unsigned int uModules31To00Bitmask;
} SIOMM_ModuleQuality;

/////////////////////////////////////////////////////////////////////
// EPIC Channel Quality 0xffff f100 2080
//
typedef struct
{
	unsigned int uaryChannelQuality[64];
} SIOMM_ChannelQuality;

/////////////////////////////////////////////////////////////////////
// EPIC Digital Channel Read Area 0xffff f01e 0000
//
typedef struct 
{
	union {
		struct {
			unsigned int uState;
			unsigned int uOnLatch;
			unsigned int uOffLatch;
			unsigned int uFeatureType;
			union
			{
				unsigned uValue;
				float fValue;
			} un;
			unsigned int uQualityDetail;
		} uAttributes;
		unsigned int uarySize[16];
	} un;
} SIOMM_EpicDigitalChannelRead;

/////////////////////////////////////////////////////////////////////
// EPIC Discrete Packed Read Area 0xffff f019 0000
//
typedef struct
{
	union {
		struct
		{
			unsigned int uStates;
			unsigned int uQuality;
			unsigned int uOnLatch;
			unsigned int uOffLatch;
		} mod;
		unsigned int uarySize[4];
	} un;
} SIOMM_EpicPackedDigitalRead;



/////////////////////////////////////////////////////////////////////
// EPIC Analog Channel Read Area 0xffff f026 0000 (modified from SNAP-PAC era devices)
//
typedef struct 
{
	union u
	{
		struct ch {
			float fEu;
			float fNotUsed0;
			float fMinimumEu;
			float fMaximumEu;
			unsigned int uQualityDetail;
		};
		unsigned int uarySize[16];
	};
} SIOMM_EpicAnalogChannelRead;

/////////////////////////////////////////////////////////////////////
// Custom Area Addresses; SNAP-PAC Contains 1024 elements. 0xffff f0d5 0000
//
typedef struct 
{
	int i32aryAddresses[256];     ///< OptoMMP addresses. Up to 256 per transaction.
} SIOMM_CustomAreaAddresses;

/////////////////////////////////////////////////////////////////////
// Custom Area Data; SNAP-PAC Contains 1024 elements. 0xffff f0d6 0000
//
typedef struct 
{
	union u {
		int   i32aryValues[256]; ///< Data values as integers, union allows read/writes to process ints and floats at once
		float f32aryValues[256]; ///< Data values as floats, union allows read/writes to process ints and floats at once
	};
} SIOMM_CustomAreaData;

#endif // __O22STRCT_H
