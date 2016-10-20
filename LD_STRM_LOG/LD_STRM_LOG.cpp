#include <LD_STRM_LOG.h>

/*

*/



//  Constructor
LD_STRM_LOG::LD_STRM_LOG(byte theUnit, byte theDeviceNumber, boolean serialEnabled, int theSPIPort)
{
    setCurrFunction(50);
    deviceBegin(theUnit, theDeviceNumber, serialEnabled);
    // initialise the RTC
    rtc.begin(DateTime(F(__DATE__), F(__TIME__)));
    // Get the clock
    now = rtc.now();
    if (!SD.begin(theSPIPort)) 
    {
        setError(-10);
        setEnabled(false);
    }
    else
    {
        setEnabled(true);
    }
}

//  Destructor
LD_STRM_LOG::~LD_STRM_LOG()
{
    setCurrFunction(0);
    /* nothing to do */
}

void LD_STRM_LOG::streamBegin(boolean sendSerial)
{
    setCurrFunction(51);
    //xBee = SoftwareSerial xBee(theRxPort, theTxPort);
    mySendSerial = sendSerial;
    myFilename = streamNameGen();
    myHandle = SD.open(myFilename, FILE_WRITE);
    if (myHandle) 
    {
        setActive(true);     
    }
    else
    {
        setError(-11);
        setActive(false);
    } 
}

void LD_STRM_LOG::streamEnd() 
{
    setCurrFunction(52);
    if (myHandle) 
    {
        myHandle.close();
    }
    setActive(false);
}

byte LD_STRM_LOG::MaxBuff()
{
    return(MyMaxBuff);
}

byte LD_STRM_LOG::outputInit(char *theBuff, byte theMaxIX)
// also initialises the 
// returns the length of the buffer as confirmation, or an error (<0)
{
    int i;
    setCurrFunction(53);

    myOutputMax = theMaxIX;
    for (i = 0; i < (myOutputMax + 1); i++)
    {
        *(theBuff + i) = MyNull;
    }
    myOutputIX = 0;
    return(theMaxIX);
}

byte LD_STRM_LOG::outputFill(char *theSource, char *theBuff, byte theBuffOffset, byte theLength)
// This does a straight copy of the theSource to the output buffer after some checking
{
    byte i;
    setCurrFunction(54);

    // if the device is active...
    if (isActive())
    {   
        // check we would not go past the end of the buffer
        if ((theBuffOffset + theLength) > (myOutputMax + 1))
        {
            // would extend past end of buffer
            return(setError(-11));
        }
        else
        {   // all good - go do the copy
            for (i = 0; i < theLength; i++)
            {
                *(theBuff + theBuffOffset + i) = *(theSource + i);
            }
            // return the bytes copied
            /*
            print("OutputCopy=");
            printArray((theBuff+theBuffOffset), theLength, true);
            print(".");
            */
            return(theLength);
        }

    } 
    else 
    {
        return(setError(-12));
    }
}

byte LD_STRM_LOG::outputBuild(char *theSource, char *theBuff, byte theLen)
// This does a copy of the source to the output buffer, but also maintains an index of where the next available 
// byte is located in the buffer - returns the number of bytes copied
{
    setCurrFunction(55);
    myOC = 0;
    // go fill the output buffer after checking it will not overflow
    myOC = outputFill(theSource, theBuff, myOutputIX, theLen);  // returns an error (<0) or the number of bytes copied
    //memcpy(&myBufferOutput[myOutputIx], &myWorkValue, theLen);
    // of successful, increment the index by the bytes copied
    if (myOC > 0) { myOutputIX += myOC; };
    return(myOC);
}

byte LD_STRM_LOG::outputSend(char *theBuff, char theType, byte theLength)
//  theLength is the length of the contents in bufferOutput, not the entire length of the datagram.  We will calculate the latter.
{
    int i;
    int j = 0; // initially used to calculate the number of values in bufferOutput, but later re-used to accumulate the number of chars sent.
    
    //if (_logComms) { _ser.print("outputSend: '"); }
    
    setCurrFunction(56);
    myOC = 0;

    if (isActive())
    {   
        // Get the clock
        timeBufferFill();
        for (i = 0; i < 14; i++)
        {
            myHandle.print(tbuf[i]);
        }

        if (mySendSerial) { printArray(&tbuf[0], 14); }
        myHandle.print(FlComma);
        if (mySendSerial) { print(FlComma, false); }
        myHandle.print(theType);
        if (mySendSerial) { print(theType, false); }
        myHandle.print(FlComma);
        if (mySendSerial) { print(FlComma, false); }

        j = 17; // need to work out what this is.
        
        // output the buffer - look for \0 and output DgSep & \0 after it
        for (i=0; i < theLength; i++) 
        {
            myHandle.print(*(theBuff + i));
            if (mySendSerial) { print(*(theBuff + i), false); }
            j++;
        }
        myHandle.println();
        if (mySendSerial) { print(' ', true); }
        myHandle.flush();
        myOC = j;
    } 
    else
    {
        // what happens if not active?
        myOC = -15;
        setError(myOC);
    }
    return(myOC);
}
