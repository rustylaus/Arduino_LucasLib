#include "LD_BASE.h"

/*
    LD_BASE();                                           // Constructor
    ~LD_BASE();                                          // De-Constructor
    void deviceInit(String theUnit, byte theDeviceNumber, boolean serialEnabled) // base initialiser
    void setDevice(byte theNumber);                     // sets this device number
    byte device();                                      // gets the device number
    void setCurrFunction(byte theFunction);             // sets the current function number
    byte currFunction();                                // gets the current function
    // Enabled indicates whether the device is connected, on and able to be used
    // Active indicates whether the device is currently being used.  A device must be enabled before it can ever be active.
    void setEnabled(boolean isEnabled);                 // sets the enabled status
    boolean isEnabled();                                // returns whether the device is enabled
    byte enabled();                                     // returns 1 if enabled or 0 if not
    void setActive(boolean isActive);                   // sets the active status
    boolean isActive();                                 // returns whether the device is active
    byte active();                                      // returns 1 if active or 0 if not
    byte setError(byte theError);                       // indicates a new error
    boolean newError(boolean resetNewError = true);     // Indicates if a new error has just occurred
    byte error(byte theIndex = 0);                      // returns the error indicated by theIndex, the latest error being the default
    byte errorFnc(byte theIndex = 0);                   // returns the error function indicated by the index
    void printOn(long thePortSpeed);
    void printOff();
    void print(String theString, boolean newLine = false);                              // Prints a String
    void print(int theInt, boolean newLine = false, boolean isDec = false);             // Prints an integer, optionally as DEC
    void print(unsigned int theInt, boolean newLine = false, boolean isDec = false);    // Prints an unsigned integer, optionally as DEC
    void print(char theChar, boolean newLine = false, boolean isHex = false);           // Prints a char, optionally as HEX
    void printArray(char *theChar, int theLen, boolean isHex = false);                  // Prints char array for specified length
    void printStat(String theString, int theStatus);
    void dumpErrInfo();
    void dumpDevInfo();   
*/

String myUnit;
byte myDeviceNumber = 0;
byte myCurrFunction = 0;
boolean myDeviceInitialised = false;
boolean myEnabled = false;
boolean myActive = false;
boolean myNewError = false;
boolean mySerialEnabled = false;
boolean mySerialActive = false;
byte myError[ErrorArraySize];
byte myErrorFnc[ErrorArraySize];

String strTF(boolean theValue)
{
    if (theValue)
    {
        return("true");
    }
    else
    {
        return("false");
    }
}

String strFmtByte(byte theValue, String theSuffix)
{
    char myFmt[4];
    String myStr = StrNull;
    sprintf(myFmt,Fmt3u,theValue);
    myStr.concat(myFmt);
    myStr.concat(theSuffix);
    return(myStr);
}

//  Constructor
LD_BASE::LD_BASE()
{
    int i;

    myCurrFunction = 0;
    myEnabled = false;
    myActive = false;
    myUnit = StrNull;
    myDeviceNumber = 0;
    mySerialEnabled = false;
    mySerialActive = false;

    for (i = 0; i < ErrorArraySize; i++)
    {
        myError[i] = 0;
        myErrorFnc[i] = 0;
    }
}

//  Destructor
LD_BASE::~LD_BASE()
{
    /* nothing to do */
    myCurrFunction = 1;
}
//  Initialiser
void LD_BASE::deviceInit(String theUnit, byte theDeviceNumber, boolean serialEnabled)
{
    myCurrFunction = 2;
    myUnit = theUnit;
    myDeviceNumber = theDeviceNumber;
    mySerialEnabled = serialEnabled;
    myDeviceInitialised = true;
}

//  Get Unit Number
String LD_BASE::unit()
{
    myCurrFunction = 3;
    if (myDeviceInitialised)
    {
        return(myUnit);
    }
    else
    {
        return("!");
    }
}

//  Get Device Number
byte LD_BASE::device()
{
    myCurrFunction = 4;
    if (myDeviceInitialised)
    {
        return(myDeviceNumber);
    }
    else
    {   
        return(setError(-1));
    }
}

//  Set Current Function
void LD_BASE::setCurrFunction(byte theFunction)
{
    myCurrFunction = theFunction;
}

//  Get Current Function
byte LD_BASE::currFunction()
{
    myCurrFunction = 5;
    return(myCurrFunction);
}

//  Set Device Enabled
void LD_BASE::setEnabled(boolean isEnabled)
{
    myCurrFunction = 6;
    myEnabled = isEnabled;
    if (!isEnabled)
    {
        myActive = false;
    }
}

//  Returns whether the device is enabled
boolean LD_BASE::isEnabled()
{
    myCurrFunction = 7;
    return(myEnabled);
}

//  Similar to isEnabled, except that it returns 1 or zero rather than true or false.
byte LD_BASE::enabled()
{
    myCurrFunction = 8;
    if (isEnabled())
    {
        return(1);
    }
    else
    {
        return(0);
    }
}

//  Sets the active status
void LD_BASE::setActive(boolean isActive)
{
    myCurrFunction = 9;
    if (myDeviceInitialised)
    {
        if (isEnabled())
        {
            myActive = isActive;
        } else {
            // device is not active and therefore the active state cannot be setActive
            ;
        }
    }
    else
    {
        // Not initialised
        setError(-1);
    }
}

//  Returns whether the device is active
boolean LD_BASE::isActive()
{
    myCurrFunction = 10;
    return(myActive);
}

//  Similar to isActive, except that it returns 1 or zero rather than true or false.
byte LD_BASE::active()
{
    myCurrFunction = 11;
    if (myActive)
    {
        return(1);
    }
    else
    {
        return(0);
    }
}

//  Indicates a new error
byte LD_BASE::setError(byte theErrorNo)
{
    // Update the structure first
    myError[2] = myError[1];
    myError[1] = myError[0];
    myError[0] = abs(theErrorNo);
    myErrorFnc[2] = myErrorFnc[1];
    myErrorFnc[1] = myErrorFnc[0];
    myErrorFnc[0] = myCurrFunction;

    myNewError = true;

    return(theErrorNo);
}

//  Returns true or false depending on if a new error was reported.              
boolean LD_BASE::newError(boolean resetNewError)
//  default:= resetNewError = true
{
    myCurrFunction = 12;
    boolean myNewErr = myNewError;
    if (resetNewError) { myNewError = false; }
    return (myNewErr);
}

//  Returns the error indicated by theIndex, the latest error being the default               
byte LD_BASE::error(byte theIndex)
//  default:= theIndex = 0
{
    myCurrFunction = 13;
    if (theIndex > -1 && theIndex < ErrorArraySize)
    {
        return(myError[theIndex]);
    } else {
        return(0);
    }
}

//  Returns the function that errored indicated by theIndex, the latest error being the default               
byte LD_BASE::errorFnc(byte theIndex)
//  default:= theIndex = 0
{
    myCurrFunction = 14;
    if (theIndex > -1 && theIndex < ErrorArraySize)
    {
        return(myErrorFnc[theIndex]);
    } else {
        return(0);
    }
}

//  Serial device initialisation
void LD_BASE::printOn(long thePortSpeed)
{
    myCurrFunction = 15;
    // check that I am enabled first
    if (myDeviceInitialised)
    {
        // the sequence of the next statement may seem illogical
        // we turn in the Active status even if this instances serial may not be enabled
        // because another instance (usually the unit instance) would have initialised 
        // the serial port anyway.  All we really need is for this instance to be
        // initialised.
        mySerialActive = true;
        //
        if (mySerialEnabled)
        {
            Serial.begin(thePortSpeed);
            while (!Serial) 
            {
                ; // wait for serial port to connect. Needed for native USB port only
            }
            // set the underlying device base class to beoing active
        } 
        else
        {
            /* do nothing */
            ;
        }
    }
    else
    {
        // Not initialised
        setError(-1);
    }
}

//  Serial device off
void LD_BASE::printOff()
{
    myCurrFunction = 16;
    mySerialActive = false;
}

//  Print a string to the serial port
void LD_BASE::print(String theString, boolean newLine)
//  default:= newLine = false
{
    myCurrFunction = 17;
    if (mySerialActive) 
    {
        if (newLine) 
        {
            Serial.println(theString);
            delay(200);
        } else 
        {
            Serial.print(theString);
        }
        delay(10);
    }
}

//  Print an integer to the serial port, optionally as DEC
void LD_BASE::print(int theInt, boolean newLine, boolean isDec)
// default:= newLine = false, isDec = false
{
    myCurrFunction = 18;
    if (mySerialActive)
    {
        if (newLine) 
        {
            if (isDec)
            {
                Serial.println(theInt, DEC);
            } else
            {
                Serial.println(theInt);
            }
        } else 
        {
            if (isDec)
            {
                Serial.print(theInt, DEC);                
            } else
            {
                Serial.print(theInt);
            }
        }
        delay(10); 
    }
}

//  Print an unsigned integer to the serial port, optionally as DEC
void LD_BASE::print(unsigned int theInt, boolean newLine, boolean isDec)
// default:= newLine = false, isDec = false
{
    myCurrFunction = 19;
    if (mySerialActive)
    {
        if (newLine) 
        {
            if (isDec)
            {
                Serial.println(theInt, DEC);
            } else
            {
                Serial.println(theInt);
            }
        } else 
        {
            if (isDec)
            {
                Serial.print(theInt, DEC);                
            } else
            {
                Serial.print(theInt);
            }
        }
        delay(10); 
    }
}

//  Print a character to the serial port, optionally as HEX
void LD_BASE::print(char theChar, boolean newLine, boolean isHex)
// default:= newLine = false, isDec = false
{
    myCurrFunction = 20;
    if (mySerialActive)
    {
        if (newLine) 
        {
            if (isHex)
            {
                Serial.println(theChar, HEX);
            } else
            {
                Serial.println(theChar);
            }
        } else 
        {
            if (isHex)
            {
                Serial.print(theChar, HEX);                
            } else
            {
                Serial.print(theChar);
            }
        }
        delay(10); 
    }
}

/*
//  Print a char array to the serial port for the specified length
void LD_BASE::printArray(char *theChar, int theLen, boolean isHex)
//  default:= isHex = false
{
    myCurrFunction = 21;
    if (mySerialActive)
    {
        int i;
        for (i = 0; i < theLen; i++) 
        {
            print(*(theChar + i), false, isHex);
        }
    }

}

//  Print a standard status message
void LD_BASE::printStat(String theString, int theStatus)
{
    myCurrFunction = 22;
    if (mySerialActive) 
    {
        Serial.print("INFO> ");
        Serial.print(theString);
        Serial.print(", Status = ");
        Serial.println(theStatus);
        delay(10);
    }
}
*/

String LD_BASE::dumpErrInfo()
{
    myCurrFunction = 23;
    char myFmt[4];
    String strErr = "Errs: New=";
    strErr.concat(strTF(myNewError));
    strErr.concat(", E0=");
    strErr.concat((strFmtByte(myError[0], "@")));
    //sprintf(myFmt,Fmt3u,myError[0]);
    //strErr.concat(myFmt);
    //strErr.concat("@");
    strErr.concat((strFmtByte(myErrorFnc[0], ", E1=")));
    //sprintf(myFmt,Fmt3u,myErrorFnc[0]);
    //strErr.concat(myFmt);
    //strErr.concat(", E1=");
    strErr.concat((strFmtByte(myError[1], "@")));
    //sprintf(myFmt,Fmt3u,myError[1]);
    //strErr.concat(myFmt);
    //strErr.concat("@");
    strErr.concat((strFmtByte(myErrorFnc[1], ", E2=")));
    //sprintf(myFmt,Fmt3u,myErrorFnc[1]);
    //strErr.concat(myFmt);
    //strErr.concat(", E2=");
    strErr.concat((strFmtByte(myError[2], "@")));
    //sprintf(myFmt,Fmt3u,myError[2]);
    //strErr.concat(myFmt);
    //strErr.concat("@");
    strErr.concat((strFmtByte(myErrorFnc[2], ".")));
    //sprintf(myFmt,Fmt3u,myErrorFnc[2]);
    //strErr.concat(myFmt);
    //strErr.concat(".");
    return(strErr);
}

String LD_BASE::dumpDevInfo()
{
    char myFmt[4];
/*
    String strErr = "DEVDUMP>> Unit=";
    strErr.concat(myUnit);
    strErr.concat(", Dev=");
    sprintf(myFmt,Fmt3u,myDeviceNumber);
    strErr.concat(myFmt);
    strErr.concat(", Fnc=");
    sprintf(myFmt,Fmt3u,myCurrFunction);
    strErr.concat(myFmt);
    strErr.concat(", Init=");
    strErr.concat(strTF(myDeviceInitialised));
    strErr.concat(", Enbld=");
    strErr.concat(strTF(myEnabled));
    strErr.concat(", Actv=");
    strErr.concat(strTF(myActive));
    strErr.concat(", SerEn=");
    strErr.concat(strTF(mySerialEnabled));
    strErr.concat(", SerAc=");
    strErr.concat(strTF(mySerialActive));
    strErr.concat("; ");
    strErr.concat(dumpErrInfo());
*/
    String strDmp = "Un=";
    strDmp.concat(myUnit);
    strDmp.concat(", Dv=");
    strDmp.concat((strFmtByte(myDeviceNumber, ", Fn=")));
    //sprintf(myFmt,Fmt3u,myDeviceNumber);
    //strErr.concat(myFmt);
    //strErr.concat(", Fn=");
    strDmp.concat((strFmtByte(myCurrFunction, ", I=")));
    //sprintf(myFmt,Fmt3u,myCurrFunction);
    //strErr.concat(myFmt);
    //strErr.concat(", I=");
    strDmp.concat(strTF(myDeviceInitialised));
    strDmp.concat(", En=");
    strDmp.concat(strTF(myEnabled));
    strDmp.concat(", Ac=");
    strDmp.concat(strTF(myActive));
    strDmp.concat(", sEn=");
    strDmp.concat(strTF(mySerialEnabled));
    strDmp.concat(", sAc=");
    strDmp.concat(strTF(mySerialActive));
    strDmp.concat("; ");
    strDmp.concat(dumpErrInfo());

    return(strDmp);        
}