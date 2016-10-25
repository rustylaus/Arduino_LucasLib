//
//  HARDWARE SERIAL COMMS SUB_SYSTEM HEADER FILE
//
//  (c) Lucas Design & RustyL Ventures P/L, 2016.  All rights reserved.
//

#ifndef LD_STRM_HSER_H
#define LD_STRM_HSER_H

#include <Arduino.h>
//#include <SoftwareSerial.h>
#include <LD_BASE.h>

// Communications Static Values    
static char DgStart = '{';
static char DgEnd = '}';
static char DgSep = '|';
// Maximum Array Dimensions
const byte MyMaxToken = 31;     // the maximum token IX
const byte MyMaxBuff = 64;      // the maximum buffer IX

class LD_STRM_HSER : public LD_BASE
{

    char myBuff[65];          // Area to read I/O chars into from a device
    byte myBuffLen;           // The number of chars in the buffer
    char myToken[32];         // An area to extract information from a buffer
    byte myTokenLen;          // The number of chars in token (excluding MyNull)
    byte myNextOffset;        // The offset of the next byte to read from the buffer
    byte myOutputMax = 0;
    byte myOutputIX = 0;
    byte myOC = 0;
    char myCommType = '*';

public:
    LD_STRM_HSER(byte theUnit, byte theDeviceNumber);
    ~LD_STRM_HSER();
    void streamBegin(long thePortSpeed);
    byte outputInit(char *theBuffer, byte theMaxIX);
    byte outputBuild(char *theSource, char *theBuff, byte theLen);                          // commOutputBld
    byte outputFill(char *theSource, char *theBuff, byte theBuffOffset, byte theLength);
    byte outputSend(char *theBuff, char theType, byte theLength);                           // commsBufferSend
    void outputSendChar(char theChar);                                  // poss internal    // commsCharSend
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

};

#endif