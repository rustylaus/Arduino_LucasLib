#include <LD_MSG.h>

/*
    LD_MSG();
    ~LD_MSG();
    void init(String theUnit);
    void setVerbose(boolean isVerbose);
    boolean isVerbose();
    String unit();
    String originator();
    String msgNo();
    String tokenValue();
    void newMessage(String theObject, int theMsgNo);
    void addValue(String theString);
    void addValue(int theInt, char *theFormat);
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

String LD_MSG::unit()
{
    setCurrFunction(6);
    return(myUnit);
}


String LD_MSG::originator()
{
    setCurrFunction(7);
    return(myOriginator);
}

String LD_MSG::msgNo()
{
    setCurrFunction(8);
    return(myMsgNo);
}

String LD_MSG::tokenValue()
{
    setCurrFunction(9);
    return(myTokenValue);
}
//  Print a string to the serial port
void LD_MSG::newMessage(String theOriginator, int theMsgNo)
{   
    char myValue[10];

    setCurrFunction(6);
    if (isActive()) 
    {
        myOriginator = theOriginator;
        sprintf(myValue, Fmt3u, theMsgNo);
        myMsgNo = myValue;
        myTokenValue = StrNull;
        tokenCount = 0;
    }
}

//  Print an integer to the serial port, optionally as DEC
void LD_MSG::addValue(String theString)
{
    setCurrFunction(11);
    if (isActive())
    {   
        // we do need a check to ensure the token length does not exceed the max message length
        if ( (myUnit.length() + myOriginator.length() + myMsgNo.length() + myTokenValue.length() + theString.length()) > MessageMax)
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
void LD_MSG::addValue(int theInt, char *theFormat)
{
    char myValue[5];
    char myFormat[5];
    String myStr = StrNull;

    setCurrFunction(12);
    if (isActive())
    {
        int i;
        for (i=0; i<6; i++)
        {
            myFormat[i] = *(theFormat + i);
        }
        sprintf(myValue, myFormat, theInt);
        myStr.concat(myValue);
        if ( (myUnit.length() + myOriginator.length() + myMsgNo.length() + myTokenValue.length() + myStr.length()) > MessageMax)
        {
            setError(-15);
        } 
        else
        {
            if (tokenCount < MsgTokenMaxIX)
            {
                myTokenValue.concat(myValue);
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
