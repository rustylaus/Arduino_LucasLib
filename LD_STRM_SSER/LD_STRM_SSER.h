//
//  SOFTWARE SERIAL COMMS SUB_SYSTEM HEADER FILE
//
//  (c) Lucas Design & RustyL Ventures P/L, 2016.  All rights reserved.
//

#ifndef LD_STRM_SSER_H
#define LD_STRM_SSER_H

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <LD_BASE.h>

// Communications Static Values    
static char DgStart = '{';
static char DgEnd = '}';
static char DgSep = '|';
// Maximum Array Dimensions
const byte MyMaxToken = 31;     // the maximum token IX
const byte MyMaxBuff = 64;      // the maximum buffer IX

class LD_STRM_SSER : public LD_BASE
{

    //char myBuff[65];          // Area to read I/O chars into from a device
    byte myBuffLen;           // The number of chars in the buffer
    char myToken[32];         // An area to extract information from a buffer
    byte myTokenLen;          // The number of chars in token (excluding MyNull)
    byte myNextOffset;        // The offset of the next byte to read from the buffer
    byte myOutputMax = 0;
    byte myOutputIX = 0;
    byte myOC = 0;
    char myCommType = '*';
    boolean myLogComms = false;

public:

    char buff[65];

    LD_STRM_SSER(byte theUnit, byte theDeviceNumber, boolean serialEnabled, int theRxPort, int theTxPort);
    ~LD_STRM_SSER();
    void streamBegin(long thePortSpeed, boolean sendSerial = false);
    void streamListen();
    byte outputInit(char *theBuff, byte theMaxIX);
    byte outputBuild(char *theSource, char *theBuff, byte theLen);                          // commOutputBld
    byte outputFill(char *theSource, char *theBuff, byte theBuffOffset, byte theLength);
    byte outputSend(char *theBuff, char theType, byte theLength);                           // commsBufferSend
    void outputSendChar(char theChar);                                  // poss internal    // commsCharSend
    void bufferCopy(char *theDestinationBuff , byte theLength);
    int inputAvailable();
    byte inputRecv(boolean discard = false);                                                // commsBufferGet
    byte inputLen();
    byte tokenGetLen(int theLength, int theOffset = 999);                                   // buffCharsGetLen
    byte tokenGetSep(int theOffset = 999);                                                  // buffCharsGetSep
    char tokenChar(byte theIX);
    int tokenInt();
    String tokenString();
    byte tokenLen();
    char dgType();
    byte MaxToken();
    byte MaxBuff();

private:
    SoftwareSerial sSER;

};

#endif