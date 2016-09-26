//
//  SERIAL DEVICE
//
//  (c) Lucas Design & RustyL Ventures P/L, 2016.  All rights reserved.
//

#ifndef LD_SER_H
#define LD_SER_H

#include <Arduino.h>
//#include "../LD_DEV/LD_DEV.h"
#include <LD_DEV.h>

class LD_SER : public LD_DEV
{
    
public:
    LD_SER();                                                                           // Constructor
    ~LD_SER();                                                                          // De-Constructor
    void initPort(long thePortSpeed);
    void print(String theString, boolean newLine = false);                              // Prints a String
    void print(int theInt, boolean newLine = false, boolean isDec = false);             // Prints an integer, optionally as DEC
    void print(unsigned int theInt, boolean newLine = false, boolean isDec = false);    // Prints an unsigned integer, optionally as DEC
    void print(char theChar, boolean newLine = false, boolean isHex = false);           // Prints a char, optionally as HEX
    void printArray(char *theChar, int theLen, boolean isHex = false);                  // Prints char array for specified length
    void printStat(String theString, int theStatus);                                    // Prints standard status message
};

#endif
