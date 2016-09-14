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
    void initComm(long thePortSpeed);
    byte outputBuild(byte theLen);                                      // commOutputBld
    int outputSend(char theType, int theLength);                         // commsBufferSend
    void outputSendChar(char theChar);               // poss internal    // commsCharSend
    int inputRecv(boolean discard = false);             // commsBufferGet
    int tokenGetLen(int theOffset, int theLength);     // buffCharsGetLen
    int tokenGetSep(int theOffset);                    // buffCharsGetSep
    //
    // if WORK_IO is contained in class - we need functions to set/get variables
    // what about the _commbuffers?
    char type();
    char input(byte theOffset);
    char inputSet(byte theOffset, char theChar, byte theLen = 1);
    char output(byte theOffset);
    char outputSet(byte theOffset, char theChar, byte theLen = 1);
    String token();
    nextOffset
}

#endif