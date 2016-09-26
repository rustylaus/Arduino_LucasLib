//
//  DEVICE HEADER
//
//  (c) Lucas Design & RustyL Ventures P/L, 2016.  All rights reserved.
//

#ifndef LD_DEV_H
#define LD_DEV_H

#include <Arduino.h>
#include <LD_COMMON.h>

#define ErrorArraySize 3

// Device Enabled Values
const byte DevEnableOn = 1;
const byte DevEnableOff = 0;
// Device Status Values
const byte DevStatOff = 0;
const byte DevStatOn = 1;

class LD_DEV 
{
    
public:
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
 };

#endif