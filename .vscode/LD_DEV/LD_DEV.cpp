#include "LD_DEV.h"

/*
    LD_DEV();                                           // Constructor
    ~LD_DEV();                                          // De-Constructor
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
*/

byte myDeviceNumber = 0;
byte myCurrFunction = 0;
boolean myEnabled = false;
boolean myActive = false;
boolean myNewError = false;
byte myError[ErrorArraySize];


//  Constructor
LD_DEV::LD_DEV()
{
    int i;
    myEnabled = false;
    myActive = false;
    for (i = 0; i < ErrorArraySize; i++)
    {
        myError[i] = 0;
    }
}

//  Destructor
LD_DEV::~LD_DEV()
{
    /* nothing to do */
}

//  Set Device Number
void LD_DEV::setDevice(byte theNumber)
{   
    myDeviceNumber = theNumber;
}

//  Get Device Number
byte LD_DEV::device()
{
    return(myDeviceNumber);
}

//  Set Current Function
void LD_DEV::setCurrFunction(byte theFunction)
{
    myCurrFunction = theFunction;
}

//  Get Current Function
byte LD_DEV::currFunction()
{
    return(myCurrFunction);
}

//  Set Device Enabled
void LD_DEV::setEnabled(boolean isEnabled)
{
    myEnabled = isEnabled;
    if (!isEnabled)
    {
        myActive = false;
    }
}

//  Returns whether the device is enabled
boolean LD_DEV::isEnabled()
{
    return(myEnabled);
}

byte LD_DEV::enabled()
{
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
void LD_DEV::setActive(boolean isActive)
{
    if (isEnabled())
    {
        myActive = isActive;
    } else {
        // device is not active and therefore the active state cannot be setActive
        ;
    }
}

//  Returns whether the device is active
boolean LD_DEV::isActive()
{
    return(myActive);
}

byte LD_DEV::active()
{
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
byte LD_DEV::setError(byte theErrorNo)
{
    // Update the structure first
    myError[2] = myError[1];
    myError[1] = myError[0];
    myError[0] = abs(theErrorNo);

    myNewError = true;

    return(theErrorNo);
}

//  Returns true or false depending on if a new error was reported.              
boolean LD_DEV::newError(boolean resetNewError)
//  default:= resetNewError = true
{
    boolean myNewErr = myNewError;
    if (resetNewError) { myNewError = false; }
    return (myNewErr);
}

//  Returns the error indicated by theIndex, the latest error being the default               
byte LD_DEV::error(byte theIndex)
//  default:= theIndex = 0
{
    if (theIndex > -1 && theIndex < ErrorArraySize)
    {
        return(myError[theIndex]);
    } else {
        return(0);
    }
}    