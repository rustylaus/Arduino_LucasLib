//
//  MESSAGE FRAMEWORK
//
//  (c) Lucas Design & RustyL Ventures P/L, 2016.  All rights reserved.
//

#ifndef LD_MSG_H
#define LD_MSG_H

#include <Arduino.h>
//#include "../LD_DEV/LD_DEV.h"
#include <LD_BASE.h>

/****d* Collector/MessageLogging
*  NAME
*   Message Logging Constants -- Constants used by the Message Logging Sub-System  
*
*  NOTES
*    This should be reviewed for moving into a header file to share between Units.
*    In this event, care should be taken to remove module specific values.
*    
*  SEE ALSO
*    None.
*
*  SOURCE
*/

//
// Messaging Sub-System Constants and Structures
//
// Message Severity Levels
/*
const char MsgSevNA[] = "-";
const char MsgSevDebug[] = "0";
const char MsgSevInfo[] = "1";
const char MsgSevErrWarning[] = "2";
const char MsgSevErrDegraded[] = "3";
const char MsgSevErrFatal[] = "4";
*/

//const boolean MsgSendIt = true;
//const boolean MsgCheckVerbose = true;

//const byte MsgOverhead = 15;



// Message Parsing
static char MsgValueSep = '^';
static char MsgToken[] = ":$:";
static byte MsgTokenLen = 3; // refers to the length of the above string
static byte MsgTokenMax = 8;
static byte MsgTokenMaxIX = 31;
static byte MessageMax = 64;

class LD_MSG : public LD_BASE
{
    
    boolean myVerbose = false;
    byte tokenCount = 0;
    byte myMsgDevice;
    byte myMsgMethod;
    int myMsgNo;
    char myTokenValue[32];
    byte myTokenIX;

public:
    LD_MSG(byte theUnit, byte theDeviceNumber, boolean serialEnabled);
    ~LD_MSG();
    void setVerbose(boolean isVerbose);
    boolean isVerbose();
    byte msgDevice();
    byte msgMethod();
    int msgNo();
    byte tokenValue(char *theCharArray);
    byte tokenLen();
    void newMessage(byte theDevice, byte theMethod, int theMsgNo);
    void addValue(char *theValueArray, byte theValueLength);
    void addValue(int theInt, char *theFormat);
};

#endif