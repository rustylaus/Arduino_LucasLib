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

//  Constructor
LD_MSG::LD_MSG(byte theUnit, byte theDeviceNumber, boolean serialEnabled)
{
    setCurrFunction(99);
    deviceBegin(theUnit, theDeviceNumber, serialEnabled);
    setEnabled(true);
    myMsgDevice = 0;
    myMsgMethod = 0;
    myMsgNo = 0;
    myTokenValue[0] = MyNull;
    myTokenIX = 0;
    tokenCount = 0;
    setActive(true);
}

//  Destructor
LD_MSG::~LD_MSG()
{
    setCurrFunction(0);
    /* nothing to do */
}

//  Sets the verbose setting
void LD_MSG::setVerbose(boolean isVerbose)
{   
    setCurrFunction(98);
    myVerbose = isVerbose;
}

// retrieves the current verbose setting
boolean LD_MSG::isVerbose()
{   
    return(myVerbose);
}

byte LD_MSG::msgDevice()
{
    return(myMsgDevice);
}

byte LD_MSG::msgMethod()
{
    return(myMsgMethod);
}

int LD_MSG::msgNo()
{
    return(myMsgNo);
}

byte LD_MSG::tokenValue(char *theCharArray)
{
    setCurrFunction(97);
    int i = 0;
    while (myTokenValue[i] != MyNull && i < (MsgTokenMaxIX + 1))
    {
        *(theCharArray + i) = myTokenValue[i];
        i++;
    }
    *(theCharArray + i) = MyNull;
    i++;
    return(i);
}

byte LD_MSG::tokenLen()
{
    return(myTokenIX);
}
//  Print a string to the serial port
void LD_MSG::newMessage(byte theDevice, byte theMethod, int theMsgNo)
{   
    char myValue[4];

    setCurrFunction(96);
    if (isActive()) 
    {
        myMsgDevice = theDevice;
        myMsgMethod = theMethod;
        myMsgNo = theMsgNo;
        myTokenValue[0] = MyNull;
        myTokenIX = 0;
        tokenCount = 0;
    }
}

//  Print an integer to the serial port, optionally as DEC
void LD_MSG::addValue(char *theValueArray, byte theValueLength)
{
    setCurrFunction(95);
    if (isActive())
    {   
        // we do need a check to ensure the token length does not exceed the max message length
        if ( (13 + (sizeof(myTokenValue)) + theValueLength + 1) > MessageMax)
        {
            setError(-10);
        } 
        else
        {
            if (tokenCount > MsgTokenMax)
            {   
                setError(-11);
            }
            else
            {
                if ((myTokenIX + theValueLength + 2) > MsgTokenMaxIX) // add 1 for the separator
                {
                    setError(-12);
                }
                else
                {
                    memcpy(&myTokenValue[myTokenIX], theValueArray, (theValueLength - 1));
                    tokenCount ++;
                    myTokenIX = myTokenIX + (theValueLength-1);
                    myTokenValue[myTokenIX] = MsgValueSep;
                    myTokenIX++;
                    myTokenValue[myTokenIX] = MyNull;
                }
            }
        }
    }
}

void LD_MSG::addValue(int theInt, char *theFormat)
{
    char myValue[5];
    char myFormat[5];
    setCurrFunction(94);
    if (isActive())
    {
        memcpy(&myFormat, theFormat, 5);
        sprintf(myValue, myFormat, theInt);
        if ( (13 + (sizeof(myTokenValue)) + sizeof(myValue) + 1) > MessageMax)
        {
            setError(-15);
        } 
        else
        {
            if (tokenCount > MsgTokenMax)
            {   
                setError(-16);
            }
            else
            {
                if ((myTokenIX + sizeof(myValue) + 2) > MsgTokenMaxIX) // add 1 for the separator
                {
                    setError(-12);
                }
                else
                {
                    memcpy(&myTokenValue[myTokenIX], &myValue, ((sizeof(myValue))-1));
                    tokenCount ++;
                    myTokenIX = myTokenIX + ((sizeof(myValue)) - 1);
                    myTokenValue[myTokenIX] = MsgValueSep;
                    myTokenIX++;
                    myTokenValue[myTokenIX] = MyNull;
                 }
            }
        }
        /*
        print("Msg=");
        print(myMsgNo);
        print(",TknIX=");
        print(myTokenIX);
        print(",Tkn='");
        printArray(myTokenValue, myTokenIX, true);
        print("'.", true);
        */
    }
}