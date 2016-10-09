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
const char MsgValueSep[] = "^";
const char MsgToken[] = ":$:";
const byte MsgTokenLen = 3; // refers to the length of the above string
const byte MsgTokenMaxIX = 8;  // The maximum allowable index value for the array msgTknValues.

class LD_MSG : public LD_BASE
{
    
public:
    LD_MSG(String theUnit, byte theDeviceNumber, boolean serialEnabled);
    ~LD_MSG();
    void setVerbose(boolean isVerbose);
    boolean isVerbose();
    String msgUnit();
    String msgOriginator();
    String msgNo();
    String tokenValue();
    void newMessage(String theOriginator, int theMsgNo);
    void addValue(String theString);
    void addValue(int theInt, char *theFormat);
};

#endif