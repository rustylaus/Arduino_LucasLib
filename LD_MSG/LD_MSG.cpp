#include "C:\Users\Russell\SkyDrive\_Dev\LD_Libraries\LD_MSG\LD_MSG.h"

/*
    LD_MSG();
    ~LD_MSG();
    void init(byte theUnit);
    void setVerbose(boolean isVerbose);
    boolean isVerbose();
    void newMessage(String theObject, int theMsgNo);
    void addValue(String theString);
    void addValue(int theInt, byte theLen = 4);
*/

const byte DeviceMessage = 2;
const byte MessageMax = 50;

boolean myVerbose = false;

byte tokenCount = 0;

String myUnit;
String myOriginator;
String myMsgNo;
String myTokenValue;

//  Constructor
LD_MSG::LD_MSG()
{
    setDevice(DeviceMessage);
    setCurrFunction(1);
    setEnabled(true);
}

//  Destructor
LD_MSG::~LD_MSG()
{
    setCurrFunction(2);
    /* nothing to do */
}

//  Message initialisation
void LD_MSG::init(String theUnit)
{
    setCurrFunction(3);
    myUnit = theUnit;
    myOriginator = StrNull;
    myMsgNo = StrNull;
    myTokenValue = StrNull;
    tokenCount = 0;
    setActive(true);
}

//  Sets the verbose setting
void LD_MSG::setVerbose(boolean isVerbose)
{   
    setCurrFunction(4);
    myVerbose = isVerbose;
}

// retrieves the current verbose setting
boolean LD_MSG::isVerbose()
{   
    setCurrFunction(5);
    return(myVerbose);
}

//  Print a string to the serial port
void LD_MSG::newMessage(String theOriginator, int theMsgNo)
{   
    char myValue[4];

    setCurrFunction(6);
    if (isActive) 
    {
        myOriginator = theOriginator;
        sprintf(myValue, Fmt3u, theMsgNo);
        myMsgNo = myCurrValue;
        myTokenValue = StrNull;
        tokenCount = 0;
    }
}

//  Print an integer to the serial port, optionally as DEC
void LD_MSG::addValue(String theString)
{
    setCurrFunction(7);
    if (isActive)
    {   
        // we do need a check to ensure the token length does not exceed the max message length
        if ( (myUnit.len() + myObject.len() + myMsgNo.len() + myTokenValue() + theString.len()) > MessageMax)
        {
            setError(-10);
        } 
        else
        {
            if (tokenCount < MsgTokenMaxIX)
            {
                myTokenValue.concat(theString);
                myTokenValue.concat(MsgValueSep);
                tokenCount ++;
            } 
            else
            {
                setError(-11);
            }
        }
    }
}

//  Print a character to the serial port, optionally as HEX
void LD_MSG::addValue(int theInt, char theFormat[5] = Fmt4u)
{
    char myValue[5];

    setCurrFunction(8);
    if (isActive)
    {
        sprintf(myValue, Fmt4u, theInt);
        if ( (myUnit.len() + myObject.len() + myMsgNo.len() + myTokenValue() + theString.len()) > MessageMax)
        {
            setError(-15);
        } 
        else
        {
            if (tokenCount < MsgTokenMaxIX)
            {
                myTokenValue.concat(myCurrValue);
                myTokenValue.concat(MsgValueSep);
                tokenCount ++;
            } 
            else
            {
                setError(-16);
            }
        }
    }
}
