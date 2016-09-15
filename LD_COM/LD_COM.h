//
//  COMMS SUB_SYSTEM HEADER FILE
//
//  (c) Lucas Design & RustyL Ventures P/L, 2016.  All rights reserved.
//

#ifndef LD_COM_H
#define LD_COM_H

#include <Arduino.h>
#include "LD_SER.h"

class LD_COM_H : public LD_SER
{
    
public:
    LD_COM();
    ~LD_COM();
    void initComm(int theRxPort, int theTxPort, long thePortSpeed);
    byte outputBuild(byte theLen);                                      // commOutputBld
    int outputSend(char theType, int theLength);                         // commsBufferSend
    void outputSendChar(char theChar);               // poss internal    // commsCharSend
    int inputRecv(boolean discard = false);                             // commsBufferGet
    int tokenGetLen(int theOffset, int theLength);                      // buffCharsGetLen
    int tokenGetSep();                                                  // buffCharsGetSep
    void setBufferOutput(byte theIX, char theChar);
    char bufferOutput(byte theIX);
    char token(byte theIX);
    byte tokenLen();
    char dgType();

}

#endif