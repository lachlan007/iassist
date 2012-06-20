//---------------------------------------------------------------------------
// Copyright (C) 2000 Dallas Semiconductor Corporation, All Rights Reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY,  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL DALLAS SEMICONDUCTOR BE LIABLE FOR ANY CLAIM, DAMAGES
// OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//
// Except as contained in this notice, the name of Dallas Semiconductor
// shall not be used except as stated in the Dallas Semiconductor
// Branding Policy.
//--------------------------------------------------------------------------
//
//  humutil.h - header file for humachron functions.
//  Version 2.00
//
#ifndef HUMUTIL_H
#define HUMUTIL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ownet.h"

#define MAX_READ_RETRY_CNT  15

// *****************************************************************************
//  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Register addresses and control bits
//  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// *****************************************************************************

/** Address of the Real-time Clock Time value*/
#define RTC_TIME  0x200
/** Address of the Real-time Clock Date value*/
#define RTC_DATE  0x203

/** Address of the Sample Rate Register */
#define SAMPLE_RATE  0x206 // 2 bytes, LSB first, MSB no greater than 0x3F

/** Address of the Temperature Low Alarm Register */
#define TEMPERATURE_LOW_ALARM_THRESHOLD  0x208
/** Address of the Temperature High Alarm Register */
#define TEMPERATURE_HIGH_ALARM_THRESHOLD  0x209

/** Address of the Data Low Alarm Register */
#define DATA_LOW_ALARM_THRESHOLD  0x20A
/** Address of the Data High Alarm Register */
#define DATA_HIGH_ALARM_THRESHOLD  0x20B

/** Address of the last temperature conversion's LSB */
#define LAST_TEMPERATURE_CONVERSION_LSB  0x20C
/** Address of the last temperature conversion's MSB */
#define LAST_TEMPERATURE_CONVERSION_MSB  0x20D

/** Address of the last data conversion's LSB */
#define LAST_DATA_CONVERSION_LSB  0x20E
/** Address of the last data conversion's MSB */
#define LAST_DATA_CONVERSION_MSB  0x20F

/** Address of Temperature Control Register */
#define TEMPERATURE_CONTROL_REGISTER  0x210
/** Temperature Control Register Bit: Enable Data Low Alarm */
#define TCR_BIT_ENABLE_TEMPERATURE_LOW_ALARM  0x01
/** Temperature Control Register Bit: Enable Data Low Alarm */
#define TCR_BIT_ENABLE_TEMPERATURE_HIGH_ALARM  0x02

/** Address of Data Control Register */
#define DATA_CONTROL_REGISTER  0x211
/** Data Control Register Bit: Enable Data Low Alarm */
#define DCR_BIT_ENABLE_DATA_LOW_ALARM  0x01
/** Data Control Register Bit: Enable Data High Alarm */
#define DCR_BIT_ENABLE_DATA_HIGH_ALARM  0x02

/** Address of Real-Time Clock Control Register */
#define RTC_CONTROL_REGISTER  0x212
/** Real-Time Clock Control Register Bit: Enable Oscillator */
#define RCR_BIT_ENABLE_OSCILLATOR  0x01
/** Real-Time Clock Control Register Bit: Enable High Speed Sample */
#define RCR_BIT_ENABLE_HIGH_SPEED_SAMPLE  0x02

/** Address of Mission Control Register */
#define MISSION_CONTROL_REGISTER  0x213
/** Mission Control Register Bit: Enable Temperature Logging */
#define MCR_BIT_ENABLE_TEMPERATURE_LOGGING  0x01
/** Mission Control Register Bit: Enable Data Logging */
#define MCR_BIT_ENABLE_DATA_LOGGING  0x02
/** Mission Control Register Bit: Set Temperature Resolution */
#define MCR_BIT_TEMPERATURE_RESOLUTION  0x04
/** Mission Control Register Bit: Set Data Resolution */
#define MCR_BIT_DATA_RESOLUTION  0x08
/** Mission Control Register Bit: Enable Rollover */
#define MCR_BIT_ENABLE_ROLLOVER  0x10
/** Mission Control Register Bit: Start Mission on Temperature Alarm */
#define MCR_BIT_START_MISSION_ON_TEMPERATURE_ALARM  0x20

/** Address of Alarm Status Register */
#define ALARM_STATUS_REGISTER  0x214
/** Alarm Status Register Bit: Temperature Low Alarm */
#define ASR_BIT_TEMPERATURE_LOW_ALARM  0x01
/** Alarm Status Register Bit: Temperature High Alarm */
#define ASR_BIT_TEMPERATURE_HIGH_ALARM  0x02
/** Alarm Status Register Bit: Data Low Alarm */
#define ASR_BIT_DATA_LOW_ALARM  0x04
/** Alarm Status Register Bit: Data High Alarm */
#define ASR_BIT_DATA_HIGH_ALARM  0x08
/** Alarm Status Register Bit: Battery On Reset */
#define ASR_BIT_BATTERY_ON_RESET  0x80

/** Address of General Status Register */
#define GENERAL_STATUS_REGISTER  0x215
/** General Status Register Bit: Sample In Progress */
#define GSR_BIT_SAMPLE_IN_PROGRESS  0x01
/** General Status Register Bit: Mission In Progress */
#define GSR_BIT_MISSION_IN_PROGRESS  0x02
/** General Status Register Bit: Conversion In Progress */
#define GSR_BIT_CONVERSION_IN_PROGRESS  0x04
/** General Status Register Bit: Memory Cleared */
#define GSR_BIT_MEMORY_CLEARED  0x08
/** General Status Register Bit: Waiting for Temperature Alarm */
#define GSR_BIT_WAITING_FOR_TEMPERATURE_ALARM  0x10
/** General Status Register Bit: Forced Conversion In Progress */
#define GSR_BIT_FORCED_CONVERSION_IN_PROGRESS  0x20

/** Address of the Mission Start Delay */
#define MISSION_START_DELAY  0x216 // 3 bytes, LSB first

/** Address of the Mission Timestamp Time value*/
#define MISSION_TIMESTAMP_TIME  0x219
/** Address of the Mission Timestamp Date value*/
#define MISSION_TIMESTAMP_DATE  0x21C

/** Address of Device Configuration Register */
#define DEVICE_CONFIGURATION_BYTE  0x226

// 1 byte, alternating ones and zeroes indicates passwords are enabled
/** Address of the Password Control Register. */
#define PASSWORD_CONTROL_REGISTER  0x227

// 8 bytes, write only, for setting the Read Access Password
/** Address of Read Access Password. */
#define READ_ACCESS_PASSWORD  0x228

// 8 bytes, write only, for setting the Read Access Password
/** Address of the Read Write Access Password. */
#define READ_WRITE_ACCESS_PASSWORD  0x230

// 3 bytes, LSB first
/** Address of the Mission Sample Count */
#define MISSION_SAMPLE_COUNT  0x220

// 3 bytes, LSB first
/** Address of the Device Sample Count */
#define DEVICE_SAMPLE_COUNT  0x223

// first year that calendar starts counting years from
#define FIRST_YEAR_EVER  2000

// maximum size of the mission Log
#define MISSION_LOG_SIZE  8192

// mission Log size for odd combination of resolutions (i.e. 8-bit temperature
// & 16-bit data or 16-bit temperature & 8-bit data
#define ODD_MISSION_LOG_SIZE  7680

#define TEMPERATURE_CHANNEL  0
#define DATA_CHANNEL         1

/** 1-Wire command for Clear Memory With Password */
#define CLEAR_MEMORY_PW_COMMAND     0x96
/** 1-Wire command for Start Mission With Password */
#define START_MISSION_PW_COMMAND    0xCC
/** 1-Wire command for Stop Mission With Password */
#define STOP_MISSION_PW_COMMAND     0x33
/** 1-Wire command for Forced Conversion */
#define FORCED_CONVERSION           0x55

typedef struct
{
   int      sampleRate;
   int      startDelay;
   SMALLINT enableRollover;
   SMALLINT timeSync;
   SMALLINT tempEnabled;
   SMALLINT dataEnabled;
   SMALLINT highDataResolution;
   SMALLINT highTempAlarm;
   SMALLINT lowTempAlarm;
   SMALLINT highDataAlarm;
   SMALLINT lowDataAlarm;
   SMALLINT tempAlarm;
   SMALLINT highTempResolution;
   float    highTemp;
   float    lowTemp;
   float    highData;
   float    lowData;
} startMissionData;

typedef struct
{
   int      highTemp;
   int      lowTemp;
   uchar    configByte;
   SMALLINT hasHumidity;
   SMALLINT useHumidityCalibration;
   SMALLINT useTemperatureCalibration;
   SMALLINT useTempCalforHumidity;
   int      adDeviceBits;
   double   adReferenceVoltage;
   double   humCoeffA;
   double   humCoeffB;
   double   humCoeffC;
   double   tempCoeffA;
   double   tempCoeffB;
   double   tempCoeffC;
} configLog;

SMALLINT startMission(int portnum, uchar *SNum, startMissionData set,
                      configLog *config);
SMALLINT readDevice(int portnum, uchar *SNum, uchar *buffer, configLog *config);
SMALLINT getFlag (int portnum, uchar *SNum, int reg, uchar bitMask);
uchar readByte(int portnum, uchar *SNum, int memAddr);
void setFlag (int reg, uchar bitMask, SMALLINT flagValue, uchar *state);
void setTime(int timeReg, int hours, int minutes, int seconds,
             SMALLINT AMPM, uchar *state);
void setDate (int timeReg, int year, int month, int day, uchar *state);
SMALLINT stopMission(int portnum, uchar *SNum);
double decodeHumidity(uchar *data, int length, SMALLINT reverse,
                      configLog config);
double getADVoltage(uchar *state, int length, SMALLINT reverse,
                    configLog config);
SMALLINT loadMissionResults(int portnum, uchar *SNum, configLog config);
SMALLINT doADConvert (int portnum, uchar *SNum, uchar *state);
SMALLINT doTemperatureConvert(int portnum, uchar *SNum, uchar *state);
//void setADVoltage(double voltage, uchar *data, int length);
double decodeTemperature(uchar *data, int length, SMALLINT reverse,
                         configLog config);

#ifdef __cplusplus
}
#endif

#endif
