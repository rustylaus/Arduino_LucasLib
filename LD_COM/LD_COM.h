//
//  COMMS SUB_SYSTEM HEADER FILE
//
//  (c) Lucas Design & RustyL Ventures P/L, 2016.  All rights reserved.
//

#ifndef LD_COM_H
#define LD_COM_H

#include <Arduino.h>
#include "LD_SER.h"

const char DgStart = '{';
const char DgEnd = '}';
const char DgSep = '|';

class LD_COM_H : public LD_SER
{
    
public:
    LD_COM();
    ~LD_COM();
    void commInit(int theRxPort, int theTxPort, long thePortSpeed);
    byte outputInit(char *theBuffer, byte theMaxIX);
    byte outputBuild(char *theSource, char *theBuff, byte theLen);      // commOutputBld
    byte outputFill(char *theSource, char *theBuff, byte theBuffOffset, byte theLength);
    byte outputSend(char *thebuff, char theType, int theLength);                         // commsBufferSend
    void outputSendChar(char theChar);               // poss internal    // commsCharSend
    byte inputRecv(boolean discard = false);             // commsBufferGet
    byte tokenGetLen(int theLength, int theOffset = 999);     // buffCharsGetLen
    byte tokenGetSep(int theOffset = 999);                    // buffCharsGetSep
    char tokenChar(byte theIX);
    int tokenInt();
    String tokenString();
    byte tokenLen();
    char dgType();
    byte MaxToken();
    byte MaxBuff();

}

#endif