#include <LD_MSG.h>

/*
    LD_MSG();
    ~LD_MSG();
    void setVerbose(boolean isVerbose);
    boolean isVerbose();
    String msgUnit();
    String msgOriginator();
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


String myOriginator;
String myMsgNo;
String myTokenValue;

//  Constructor
LD_MSG::LD_MSG(String theUnit, byte theDeviceNumber, boolean serialEnabled)
{
    setCurrFunction(51);
    deviceInit(theUnit, theDeviceNumber, serialEnabled);
    setEnabled(true);
    myOriginator = StrNull;
    myMsgNo = StrNull;
    myTokenValue = StrNull;
    tokenCount = 0;
    setActive(true);
}

//  Destructor
LD_MSG::~LD_MSG()
{
    setCurrFunction(52);
    /* nothing to do */
}

//  Sets the verbose setting
void LD_MSG::setVerbose(boolean isVerbose)
{   
    setCurrFunction(54);
    myVerbose = isVerbose;
}

// retrieves the current verbose setting
boolean LD_MSG::isVerbose()
{   
    setCurrFunction(55);
    return(myVerbose);
}

String LD_MSG::msgUnit()
{
    setCurrFunction(56);
    return(unit());
}

String LD_MSG::msgOriginator()
{
    setCurrFunction(57);
    return(myOriginator);
}

String LD_MSG::msgNo()
{
    setCurrFunction(58);
    return(myMsgNo);
}

String LD_MSG::tokenValue()
{
    setCurrFunction(59);
    return(myTokenValue);
}
//  Print a string to the serial port
void LD_MSG::newMessage(String theOriginator, int theMsgNo)
{   
    char myValue[10];

    setCurrFunction(60);
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
    setCurrFunction(61);
    String myUnit;
    myUnit = unit();
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
    String myUnit;
    String myStr = StrNull;

    myUnit = unit();
    setCurrFunction(62);
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
