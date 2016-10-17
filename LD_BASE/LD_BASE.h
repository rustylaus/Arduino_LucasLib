//
//  DEVICE HEADER
//
//  (c) Lucas Design & RustyL Ventures P/L, 2016.  All rights reserved.
//

#ifndef LD_BASE_H
#define LD_BASE_H

#include <Arduino.h>
#include <LD_COMMON.h>

#define ErrorArraySize 2

class LD_BASE 
{

    byte myUnit;
    byte myDeviceNumber = 0;
    byte myCurrFunction = 0;
    boolean myDeviceInitialised = false;
    boolean myEnabled = false;
    boolean myActive = false;
    boolean mySerialEnabled = false;
    boolean myNewError = false;
    byte myError[ErrorArraySize];
    byte myErrorFnc[ErrorArraySize];
    int myTotalErrors = 0;
    char fmtByteBuff[8];

public:

    LD_BASE(); // Constructor
    ~LD_BASE();                                         // De-Constructor
    void deviceBegin(byte theUnit, byte theDeviceNumber, boolean serialEnabled); // base initialiser
    byte unit();                                      // gets the unit number
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
    int totalErrors(boolean resetToZero = false);               // returns the number of errors, and resets the count if requested
    byte setError(byte theError);                       // indicates a new error
    boolean newError(boolean resetNewError = true);     // Indicates if a new error has just occurred
    byte error(byte theIndex = 0);                      // returns the error indicated by theIndex, the latest error being the default
    byte errorFnc(byte theIndex = 0);                   // returns the error function indicated by the index    
    void printBegin(long thePortSpeed);
    void printEnd();
    void print(String theString, boolean newLine = false);                              // Prints a String
    void print(int theInt, boolean newLine = false, boolean isDec = false);             // Prints an integer, optionally as DEC
    void print(unsigned int theInt, boolean newLine = false, boolean isDec = false);    // Prints an unsigned integer, optionally as DEC
    void print(char theChar, boolean newLine = false, boolean isHex = false);           // Prints a char, optionally as HEX
    void printArray(char *theChar, int theLen, boolean isHex = false);                  // Prints char array for specified length
    //void printStat(String theString, int theStatus);
    void dumpErrInfo(char *buff, byte theStartOffset);                             
    void dumpDevInfo(char *buff);

private:

    char chrTF(boolean theValue)
    // Returns a single char of value either 'T' or 'F' (true or false) based on the
    // value of the passed boolean value theValue
    {
        if (theValue)
        {
            return(ChTrue);
        }
        else
        {
            return(ChFalse);
        }
    }

    void strFmtByte(byte theValue, char *theSuffix, byte theSuffLen)
    // WARNING: Uses global member variable fmtByteBuff for output
    // Converts the numeric byte value to a 3 char null-terminated string, and copies it to
    // fmtByteBuff, then copies chars from the char array theSuffix for a length of theSuffLen,
    // to fmtByteBuff and finally adds a null tpo terminate the entire string.
    // Used to build the Dump output.
    {
        char myFmt[4];
        sprintf(myFmt,Fmt3u,theValue);
        memcpy(&fmtByteBuff[0], &myFmt[0], 3);
        memcpy(&fmtByteBuff[3], theSuffix, theSuffLen);
        fmtByteBuff[(3 + theSuffLen)] = MyNull;
    }

 };

#endif