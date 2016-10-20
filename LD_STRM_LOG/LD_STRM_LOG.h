//
//  SD STREAM & RTC SUB_SYSTEM HEADER FILE
//
//  (c) Lucas Design & RustyL Ventures P/L, 2016.  All rights reserved.
//

#ifndef LD_STRM_LOG_H
#define LD_STRM_LOG_H

#include <Arduino.h>
#include <Wire.h>             // I2C Comms Library - for RTC?
#include <SPI.h>              // SPI Comms Library - Used to communicate to the SD card
#include <SD.h>               // SD Card I/O Library
#include <RTClib.h>           // Real-time Clock Libary
#include <LD_BASE.h>

// Log Static Values    
static char FlComma = ',';

class LD_STRM_LOG : public LD_BASE
{

    byte myOutputMax = 0;
    byte myOutputIX = 0;
    byte myOC = 0;
    File myHandle;
    RTC_Millis rtc;
    DateTime now;
    String myFilename;
    boolean mySendSerial = false;
    char tbuf[15];

public:
    LD_STRM_LOG(byte theUnit, byte theDeviceNumber, boolean serialEnabled, int theSPIPort);
    ~LD_STRM_LOG();
    void streamBegin(boolean sendSerial = false);
    void streamEnd();
    byte outputInit(char *theBuff, byte theMaxIX);
    byte outputBuild(char *theSource, char *theBuff, byte theLen);                          // commOutputBld
    byte outputFill(char *theSource, char *theBuff, byte theBuffOffset, byte theLength);
    byte outputSend(char *theBuff, char theType, byte theLength);                           // commsBufferSend
    byte MaxBuff();
    
private:


    String streamNameGen()
    {
        setCurrFunction(57);
        String fn;
        int t;
        char fbuf[14];

        timeBufferFill();
        for (t=6; t < 14; t++) 
        {
            fbuf[t-6] = tbuf[t];
        }
        fbuf[8]='\0';
        fn = "";
        fn.concat(fbuf);
        fn.concat(".csv");
        return(fn);
    }

    void timeBufferFill()
    {
        setCurrFunction(58);
        now = rtc.now();
        sprintf(tbuf, "%04u%02u%02u,%02u%02u%02u",now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second());
    }

};

#endif