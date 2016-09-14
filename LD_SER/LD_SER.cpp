#include "C:\Users\Russell\SkyDrive\_Dev\LD_Libraries\LD_SER\LD_SER.h"

/*
    LD_SER();               // Constructor
    ~LD_SER();              // De-Constructor
    void initPort(long thePortSpeed);
    void print(String theString, boolean newLine = false);                          // Prints a String
    void print(int theInt, boolean newLine = false, boolean isDec = false);         // Prints an integer, optionally as DEC
    void print(char theChar, boolean newLine = false, boolean isHex = false);       // Prints a char, optionally as HEX
    void printArray(char *theChar, int theLen);                                     // Prints char array for specified length
    void printStat(String theString, int theStatus);                                // Prints standard status message
*/

const byte DeviceSerial = 1;

//  Constructor
LD_SER::LD_SER()
{   
    setCurrFunction(1);
    setDevice(DeviceSerial);
}

//  Destructor
LD_SER::~LD_SER()
{
    setCurrFunction(2);
}

//  Serial device initialisation
void LD_SER::initPort(long thePortSpeed)
{
    setCurrFunction(3);
    // check that I am enabled first
    if (isEnabled)
    {
        Serial.begin(thePortSpeed);
        while (!Serial) 
        {
        ; // wait for serial port to connect. Needed for native USB port only
        }
        // set the underlying device base class to beoing active
        setActive(true);
    } else
    {
        /* do nothing */
        ;
    }
}

//  Print a string to the serial port
void LD_SER::print(String theString, boolean newLine = false)
{
    setCurrFunction(4);
    if (isActive) 
    {
        if (newLine) 
        {
            Serial.println(theString);
        } else 
        {
            Serial.print(theString);
        }
        delay(10);
    }
}

//  Print an integer to the serial port, optionally as DEC
void LD_SER::print(int theInt, boolean newLine = false, boolean isDec = false)
{
    setCurrFunction(5);
    if (isActive)
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
void LD_SER::print(char theChar, boolean newLine = false, boolean isHex = false)
{
    setCurrFunction(6);
    if (isActive)
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

//  Print a char array to the serial port for the specified length
void LD_SER::printArray(char *theChar, int theLen, boolean isHex = false)
{
    setCurrFunction(7);
    if (isActive)
    {
        int i;
        for (i = 0; i < theLen; i++) 
        {
            print(*(theChar + i), false, isHex);
        }
    }

}

//  Print a standard status message
void LD_SER::printStat(String theString, int theStatus)
{
    setCurrFunction(8);
    if (isActive) 
    {
        Serial.print("INFO> ");
        Serial.print(theString);
        Serial.print(", Status = ");
        Serial.println(theStatus);
        delay(10);
    }
}