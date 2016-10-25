#include "LD_BASE.h"

/*
  
*/

boolean mySerialActive = false;
char fHxChr[3];

    //  Constructor
    LD_BASE::LD_BASE()
    {
        int i;

        myCurrFunction = 0;
        myEnabled = false;
        myActive = false;
        myUnit = 0;
        myDeviceNumber = 0;
        mySerialEnabled = false;
        mySerialActive = false;
        myTotalErrors = 0;
        handler = 0;

        for (i = 0; i < LD_ErrorArraySize; i++)
        {
            myError[i] = 0;
            myErrorFnc[i] = 0;
        }
        
        for (i = 0; i < LD_ActionArraySize; i++)
        {
            myAction[i] = 0;
        }
    }

    //  Destructor
    LD_BASE::~LD_BASE()
    {
        /* nothing to do */
        myCurrFunction = 0;
    }
    //  Initialiser
    void LD_BASE::deviceBegin(byte theUnit, byte theDeviceNumber, boolean serialEnabled)
    {
        myCurrFunction = 127;
        myUnit = theUnit;
        myDeviceNumber = theDeviceNumber;
        mySerialEnabled = serialEnabled;
        myDeviceInitialised = true;
    }

    //  Get Unit Number
    byte LD_BASE::unit()
    {
        myCurrFunction = 126;
        if (myDeviceInitialised)
        {
            return(myUnit);
        }
        else
        {
            return(-2);
        }
    }

    //  Get Device Number
    byte LD_BASE::device()
    {
        myCurrFunction = 125;
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
        myCurrFunction = 124;
        myCurrFunction = theFunction;
    }

    //  Get Current Function
    byte LD_BASE::currFunction()
    {
        return(myCurrFunction);
    }

    //  Set Device Enabled
    void LD_BASE::setEnabled(boolean isEnabled)
    {
        myCurrFunction = 123;
        myEnabled = isEnabled;
        if (!isEnabled)
        {
            myActive = false;
        }
    }

    //  Returns whether the device is enabled
    boolean LD_BASE::isEnabled()
    {
        return(myEnabled);
    }

    //  Similar to isEnabled, except that it returns 1 or zero rather than true or false.
    byte LD_BASE::enabled()
    {
        myCurrFunction = 122;
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
        myCurrFunction = 121;
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
        return(myActive);
    }

    //  Similar to isActive, except that it returns 1 or zero rather than true or false.
    byte LD_BASE::active()
    {
        myCurrFunction = 120;
        if (myActive)
        {
            return(1);
        }
        else
        {
            return(0);
        }
    }

    //  Returns the total number of errors since startup
    int LD_BASE::totalErrors(boolean resetToZero)
    {   
        int i;
        i = myTotalErrors;
        if (resetToZero) { myTotalErrors = 0; }
        return(i);
    }

    //  Indicates a new error
    byte LD_BASE::setError(byte theErrorNo)
    {
        /*
        print("!!");
        print((256 - theErrorNo));
        print("@");
        print(myCurrFunction, true);
        */
        // Update the structure first
        //myError[2] = myError[1];
        byte i;
        for (i=(LD_ErrorArraySize - 1); i > 0; i--)
        {
            myError[i] = myError[i - 1];
            myErrorFnc[i] = myErrorFnc[i - 1];
        }
        myError[0] = (256 - theErrorNo);
        myErrorFnc[0] = myCurrFunction;
        myTotalErrors ++;
        myNewError = true;
        return(theErrorNo);
    }

    //  Returns true or false depending on if a new error was reported.              
    boolean LD_BASE::newError(boolean resetNewError)
    //  default:= resetNewError = true
    {
        myCurrFunction = 119;
        boolean myNewErr = myNewError;
        if (resetNewError) { myNewError = false; }
        return (myNewErr);
    }

    //  Returns the error indicated by theIndex, the latest error being the default               
    byte LD_BASE::error(byte theIndex)
    //  default:= theIndex = 0
    {
        myCurrFunction = 118;
        if (theIndex > -1 && theIndex < LD_ErrorArraySize)
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
        myCurrFunction = 117;
        if (theIndex > -1 && theIndex < LD_ErrorArraySize)
        {
            return(myErrorFnc[theIndex]);
        } else {
            return(0);
        }
    }

    void LD_BASE::setAction(byte theAction, boolean theSwitch)
    {
        myCurrFunction = 115;
        if (theAction > -1 && theAction < LD_ActionArraySize)
        {
            myAction[theAction] = theSwitch;
        }
    }

    boolean LD_BASE::hasHandler()
    {
        if (handler == 0)
        {
            return(false);
        }
        else
        {
            return(true);
        }
    }
    
    boolean LD_BASE::hasAction(byte theAction)
    {
        myCurrFunction = 114;
        if (theAction > -1 && theAction < LD_ActionArraySize)
        {
            return(myAction[theAction]);
        }
        else
        {
            return(false);
        }
    }

    //  Serial device initialisation
    void LD_BASE::printBegin(long thePortSpeed)
    {
        myCurrFunction = 113;
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
    void LD_BASE::printEnd()
    {
        myCurrFunction = 112;
        mySerialActive = false;
    }

    //  Print a string to the serial port
    void LD_BASE::print(String theString, boolean newLine)
    //  default:= newLine = false
    {
        if (mySerialActive) 
        {
            if (newLine) 
            {
                Serial.println(theString);
                delay(50);
            } else 
            {
                Serial.print(theString);
            }
        }
    }

    //  Print an integer to the serial port, optionally as DEC
    void LD_BASE::print(int theInt, boolean newLine, boolean isDec)
    // default:= newLine = false, isDec = false
    {
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
                delay(10); 
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
        }
    }

    //  Print an unsigned integer to the serial port, optionally as DEC
    void LD_BASE::print(unsigned int theInt, boolean newLine, boolean isDec)
    // default:= newLine = false, isDec = false
    {
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
                delay(10); 
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
        }
    }

    //  Print a character to the serial port, optionally as HEX
    void LD_BASE::print(char theChar, boolean newLine, boolean isHex)
    // default:= newLine = false, isDec = false
    {
        if (mySerialActive)
        {
            if (newLine) 
            {
                if (isHex)
                {
                    sprintf(fHxChr,"%02X",theChar);
                    Serial.print(fHxChr[0]);
                    Serial.println(fHxChr[1]);
                } else
                {
                    Serial.println(theChar);
                }
                delay(10); 
            } else 
            {
                if (isHex)
                {
                    sprintf(fHxChr,"%02X",theChar);
                    Serial.print(fHxChr[0]);
                    Serial.print(fHxChr[1]);              
                } else
                {
                    Serial.print(theChar);
                }
            }
        }
    }


    //  Print a char array to the serial port for the specified length
    void LD_BASE::printArray(char *theChar, int theLen, boolean isHex)
    //  default:= isHex = false
    {
        if (mySerialActive)
        {
            int i;
            for (i = 0; i < theLen; i++) 
            {
                print(*(theChar + i), false, isHex);
            }
        }
        print(' ',true);

    }

    /*
    //  Print a standard status message
    void LD_BASE::printStat(String theString, int theStatus)
    {
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

#ifdef LD_ENABLE_ERR_DEBUG

    void LD_BASE::dumpErrInfo(char *buff,  byte theStartOffset)
    // buffer needs to be 43 chars, including terminating Null
    {
        char strE1[] = " Errs>Nw=";
        char strDsc[] = ",0=";
        char myStop[] = ".";
        char myFmt[2];
        byte i;
        byte p =  theStartOffset;
        strcpy((buff + p), &strE1[0]); p = p + 9;
        *(buff + p) = chrTF(myNewError); p ++;
        strcpy((buff + p), &strDsc[0]); p = p + 3;
        for (i = 0; i < ErrorArraySize; i++)
        {
            /*
            print(i, true);
            printArray(buff, 96, false);
            printArray(buff, 96, true);
            */
            strFmtByte(myError[i], &MyAt, 1);
            strcpy((buff + p), fmtByteBuff); p = p + 4;
            sprintf(myFmt, Fmt1u, (i + 1));
            strDsc[1] = myFmt[0];
            if (i < (ErrorArraySize - 1))
            {
                strFmtByte(myErrorFnc[i], &strDsc[0], 3);
                strcpy((buff + p), fmtByteBuff); p = p + 6;
            }
            else
            {
                strFmtByte(myErrorFnc[i], &myStop[0], 1);
                strcpy((buff + p), fmtByteBuff); p = p + 4;
            }
        }
        buff[p] = MyNull;
        /*
        print(i, true);
        printArray(buff, 96, false);
        printArray(buff, 96, true);
        */
    }

#endif

#ifdef LD_ENABLE_DEV_DEBUG

    void LD_BASE::dumpDevInfo(char *buff)
    // this routine requires a 85 char buffer to be passed
    {
        byte p = 2;
        char myUn[] = "U=";
        char myDv[] = ",D=";
        char myFn[] = ",F=";
        char myInit[] = ",I=";
        char myEn[] = ",E=";
        char myAc[] = ",A=";
        char mySen[] = ",sE=";
        char mySac[] = ",sA=";
        char mySc = ';';

        strcpy(buff, &myUn[0]);
        strFmtByte(myUnit, &myDv[0], 3);
        strcpy((buff + p), fmtByteBuff);  p = p + 6;
        strFmtByte(myDeviceNumber, &myFn[0], 3);
        strcpy((buff + p), fmtByteBuff);  p = p + 6;
        strFmtByte(myCurrFunction, &myInit[0], 3);
        strcpy((buff + p), fmtByteBuff); p = p + 6;
        *(buff + p) = chrTF(myDeviceInitialised);  p ++;
        strcpy((buff + p), &myEn[0]); p = p + 3;
        *(buff + p) = chrTF(myEnabled);  p ++;
        strcpy((buff + p), &myAc[0]); p = p + 3;
        *(buff + p) = chrTF(myActive);  p ++;
        strcpy((buff + p), &mySen[0]); p = p + 4;
        *(buff + p) = chrTF(mySerialEnabled);  p ++;
        strcpy((buff + p), &mySac[0]); p = p + 4;
        *(buff + p) = chrTF(mySerialActive);  p ++;
        *(buff + p) = mySc; p ++;
        dumpErrInfo(buff, p);   
    }

#endif