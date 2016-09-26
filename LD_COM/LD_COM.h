//
//  COMMS SUB_SYSTEM HEADER FILE
//
//  (c) Lucas Design & RustyL Ventures P/L, 2016.  All rights reserved.
//

#ifndef LD_COM_H
#define LD_COM_H

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <LD_SER.h>
    
const char DgStart = '{';
const char DgEnd = '}';
const char DgSep = '|';

class LD_COM : public LD_SER
{

public:
    LD_COM(int theRxPort, int theTxPort);
    ~LD_COM();
    void commInit(long thePortSpeed);
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
    SoftwareSerial xBee;

};

#endif