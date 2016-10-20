#include <LD_STRM_XBEE.h>

/*

*/

//  Constructor
LD_STRM_XBEE::LD_STRM_XBEE(byte theUnit, byte theDeviceNumber, boolean serialEnabled, int theRxPort, int theTxPort)
    : xBee(theRxPort, theTxPort)
{
    setCurrFunction(89);
    deviceBegin(theUnit, theDeviceNumber, serialEnabled);
    setEnabled(true);
}

//  Destructor
LD_STRM_XBEE::~LD_STRM_XBEE()
{
    setCurrFunction(0);
    /* nothing to do */
}

void LD_STRM_XBEE::streamBegin(long thePortSpeed, boolean sendSerial)
{
    setCurrFunction(89);
    //xBee = SoftwareSerial xBee(theRxPort, theTxPort);
    myLogComms = sendSerial;
    /*
    if (myLogComms)
    {
        print("Log comms active.");
    }
    else
    {
        print("Log comms not active");
    }
    */
    xBee.begin(thePortSpeed);
    setActive(true);
}

void LD_STRM_XBEE::streamListen()
{
    setCurrFunction(88);
        // if the device is active...
    if (isActive())
    {   
        xBee.listen();
    }
    else 
    {
        setError(-16);
    }
}

byte LD_STRM_XBEE::inputLen()
{
    return(myBuffLen);
}

char LD_STRM_XBEE::tokenChar(byte theIX)
{
    return(myToken[theIX]);
}

int LD_STRM_XBEE::tokenInt()
{
    setCurrFunction(87);
    //print("Tkn=");
    //printArray(&myToken[0],5,true);
    //print("=>");
    //int i = atoi(myToken);
    //print(i, true);
    return((atoi(myToken)));
}

String LD_STRM_XBEE::tokenString()
{   
    setCurrFunction(86);
    String s = StrNull;
    s.concat(myToken);
    return(s);
}

byte LD_STRM_XBEE::tokenLen()
{
    return(myTokenLen);
}

char LD_STRM_XBEE::dgType()
{
    return(myCommType);
}

byte LD_STRM_XBEE::MaxBuff()
{
    return(MyMaxBuff);
}

byte LD_STRM_XBEE::MaxToken()
{
    return(MyMaxToken);
}

byte LD_STRM_XBEE::outputInit(char *theBuffer, byte theMaxIX)
// also initialises the 
// returns the length of the buffer as confirmation, or an error (<0)
{
    int i;
    setCurrFunction(85);

    if (theMaxIX > MyMaxBuff)
    {
        return(setError(-10));
    }
    else
    {
        myOutputMax = theMaxIX;
        for (i = 0; i < (myOutputMax + 1); i++)
        {
            *(theBuffer + i) = MyNull;
        }
        myOutputIX = 0;
        return(theMaxIX);
    }
}

byte LD_STRM_XBEE::outputFill(char *theSource, char *theBuff, byte theBuffOffset, byte theLength)
// This does a straight copy of the theSource to the output buffer after some checking
{
    byte i;
    setCurrFunction(84);

    // if the device is active...
    if (isActive())
    {   
        // check we would not go past the end of the buffer
        if ((theBuffOffset + theLength) > (myOutputMax + 1))
        {
            // would extend past end of buffer
            return(setError(-15));
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
        return(setError(-16));
    }
}

byte LD_STRM_XBEE::outputBuild(char *theSource, char *theBuff, byte theLen)
// This does a copy of the source to the output buffer, but also maintains an index of where the next available 
// byte is located in the buffer - returns the number of bytes copied
{
    setCurrFunction(83);
    myOC = 0;
    // go fill the output buffer after checking it will not overflow
    myOC = outputFill(theSource, theBuff, myOutputIX, theLen);  // returns an error (<0) or the number of bytes copied
    //memcpy(&myBufferOutput[myOutputIx], &myWorkValue, theLen);
    // of successful, increment the index by the bytes copied
    if (myOC > 0) { myOutputIX += myOC; };
    return(myOC);
}

byte LD_STRM_XBEE::outputSend(char *theBuff, char theType, byte theLength)
//  theLength is the length of the contents in bufferOutput, not the entire length of the datagram.  We will calculate the latter.
{
    int i;
    int j = 0; // initially used to calculate the number of values in bufferOutput, but later re-used to accumulate the number of chars sent.
    
    //if (_logComms) { _ser.print("outputSend: '"); }
    
    char lenF[4];
    
    setCurrFunction(82);
    myOC = 0;

    if (isActive())
    {
        // find out how many fields there are by summing the number of NULLs in bufferOutput
        for (i=0; i < theLength; i++) 
        {
            if (*(theBuff + i) == '\0') 
            {
                j ++;
            }
        }

        // j equals the number of fields

        //  in a different field, add 2 (to allow for the separator and sep Null) for each value (defined by j) , plus one for the Type.  This calculates the entire datagram length and
        //  is used to verify the datagram at the other end.
        int lenCalc = theLength + (j * 2) + 1;
        sprintf(lenF, Fmt3u, lenCalc);

        // Output the header in the format
        //     {009\T<_commBufferOutput>}
        // where:
        //  { and } = sentinels
        //  009\ = the length of _commBufferOutput, which must include the separator strings, output as a NULL terminated String
        //  T = the dialog type
        //  <_commBufferOutput> = the structure to output
        //
        
        noInterrupts();
        // send Datagram Start
        if (myLogComms) { print(" ", true); print('$',false); }
        outputSendChar(DgStart);
        // send the datagram length
        for (i = 0; i < 4; i++) 
        {
            outputSendChar(lenF[i]);
        }
        // send the type
        outputSendChar(theType);
        // reset j to now be the count of bytes sent
        j = 5;
        
        // output the buffer - look for \0 and output DgSep & \0 after it
        for (i=0; i < theLength; i++) 
        {
            outputSendChar(*(theBuff + i));
            if (*(theBuff + i) == '\0') 
            {
                outputSendChar(DgSep);
                outputSendChar(MyNull);
                j += 2;
            }
            j ++;
        }
        outputSendChar(DgEnd);
        j ++;
        //if (_logComms) { _ser.print("'==>>>", true); }
        interrupts();
        if (myLogComms) { print(" ",true); }
        myOC = j;
    } 
    else
    {
        // what happens if not active?
        myOC = -20;
        setError(myOC);
    }
    return(myOC);
}

void LD_STRM_XBEE::outputSendChar(char theChar) 
{
    setCurrFunction(81);
    myOC = 0;
    if (myLogComms)
    {
        if (theChar < ' ')
        {
            print('.', false);
        }
        else
        {
            print(theChar, false);
        }
    }
    xBee.write(theChar);
    //if (_logComms) { _ser.print(theChar); }
}

int LD_STRM_XBEE::inputAvailable()
{
    setCurrFunction(80);
    myOC = 0;
    return(xBee.available());
}

byte LD_STRM_XBEE::inputRecv(boolean theDiscard) 
{

    // A valid datagram has the format
    //     {009\T<structure>}
    // where:
    //  { and } = sentinels
    //  009\ = the length of associated structure, which will include the separator strings, input as a NULL terminated String (\=null)
    //  T = the dialog type
    //  <_commBufferOutput> = the structure to output
    //
    // NOTE: After reading the Xbee stream, _commBufferInput contains no sentinels.  The first digit of the length is at offset zero in that buffer.

    char commChar = MyNull;
    myOC = 0;
    boolean foundStart = false;
    byte i = 0;
    byte o = 0;
    int c = 0;
    char valueF[4];
    //String strVal = StrNull;
    //char bufferInput[65];   // only used by this routine - the output is placed in myBuff

    setCurrFunction(79);
    myOC = 0;
    if (myLogComms) { print("!"); }

    for (i = 0; i < MyMaxBuff; i++) 
    {
        //bufferInput[i] = MyNull;
        myBuff[i] = MyNull;
    }
    
    //  Check the device is active
    if (isActive()) 
    {
        //noInterrupts();
        i = 0;
        //if (_logComms) { _ser.print("CommsGet: <<<=='"); }
        //_ser.print(">", false);
        //delay(500);
        while (commChar != DgEnd && i < MyMaxBuff)        // We could loop for-ever here waiting for the end sentinel - may need a circuit breaker in case of comms error
        {                                            
            c = xBee.available();
            //_ser.print(c);
            //delay(2000);
            if (c > 0) 
            {
                //delay(100);
                commChar = xBee.read();                                              // Receive a single character from the software serial port
                if (myLogComms)
                {
                    if (commChar < ' ')
                    {
                        print('.', false);
                    }
                    else
                    {
                        print(commChar, false);
                    }
                }
                /*
                if (_logComms) {
                _ser.print(_commChar);
                if (!foundStart || discard) { _ser.print('#'); }
                }
                */
                if (!theDiscard && foundStart && commChar != DgEnd) 
                {
                    //_commBufferInput.concat(_commChar);                                // Add the received character to the receive buffer ONLY if we have found the start of the dialog
                    //bufferInput[i] = commChar;
                    myBuff[i] = commChar;
                    i++;
                }
                if (!theDiscard && commChar == DgStart) { foundStart = true; }
            } 
            else 
            {
                // there were no chars - if this is a discard scenario - then that is fine - we exit.
                if (theDiscard) { commChar = DgEnd; }
            }
        }
        //if (_logComms) { _ser.print("'."); }
        //_ser.print("<");
        //_ser.print(i, true);
        if (myLogComms) { print(" ",true); }
        
        if (i < MyMaxBuff) 
        {
            if (!theDiscard) 
            {
                // dump the buffer to a char array to work on
                // load the first char (Type) to _commType
                // Get the length of the structure and load it into buffLen
                for (o = 0; o < 4; o++) 
                {
                    //valueF[o] = bufferInput[o];
                    valueF[o] = myBuff[o];
                }
                //strVal.concat(valueF);
                //myBuffLen = strVal.toInt(); // this is the length WITHOUT the overhead, but with the type!
                myBuffLen = atoi(valueF); // this is the length WITHOUT the overhead, but with the type!
                myBuffLen -= 1;             // we need buffLen to be the number of byte to copy, so subtract one for the type
                // OK, before we do anything else, we should check that the length if valid
                // the variable i has the length of the chars in _commBufferInput - we need to take off 4 for the length (incl null), and an extra one for the type
                if ( (i-5) != myBuffLen ) 
                {
                    // let the controller know the datagram was invalid
                    //ackResInvSend(DgTypeInvalid);
                    myOC = -72;        // bad length
                } 
                else 
                {
                    // the length was good!
                    // Next, get the comms type and load it into _commType
                    //myCommType = bufferInput[4];
                    myCommType = myBuff[4];
                    // Now load the remainder of the input buffer into the WORK_IO.buff array
                    for (o = 0; o < myBuffLen; o++)          // note that buffLen = the associated structure ONLY, not the initial overhead (4 char length + 1 char type)
                    {
                        myBuff[o] = myBuff[(o + 5)];
                        //myBuff[o] = bufferInput[(o + 5)];
                        //_commBufferInput.toCharArray(*ioWork->buff,100);
                    }
                    // set the outcome to equal the length of the buffer
                    myOC = myBuffLen;
                }
            } 
            else 
            {
                myOC = 0;
            } 
        } 
        else 
        {
            //*ioWork->buffLen = 0;
            myOC = -30;            // buffer overflow - no full message can be greater than 128 chars
        }
        myNextOffset = 0;
        //interrupts();
    } 
    else 
    {
        myOC = -31; 
    }

    if (myOC < 0) { setError(myOC); }
    return(myOC);
}

byte LD_STRM_XBEE::tokenGetLen(int theLength, int theOffset) 
{

    byte i;

    setCurrFunction(78);
    myOC = 0;

    if (theOffset == 999) { theOffset = myNextOffset; }

    // check we are not asking for something beyond the end of the buffer
    if ((theLength < (MyMaxToken)) && ((theOffset + theLength) < (myBuffLen + 1))) 
    {
        // initialise output buffer (token) with nulls
        for (i=0; i<(MyMaxToken); i++) 
        {
            myToken[i] = MyNull;
        }
        // move the number of characters specified
        i=0;
        while ((i < theLength))            // No longer going to check for separator [&& (ioWork->buff + (theOffset + i) != DgSep)]
        {
            myToken[i] = myBuff[(theOffset + i)];
            i++;
        }
        // set the offset to start on the next call
        myNextOffset = (theOffset + i);
        // now, make sure the token is a string
        if (myToken[(i - 1)] != MyNull) 
        {
            myToken[i] = MyNull;
            i++; // add the extra char for the \0
        }

        /*
        _ser.print("Debug> buffCharsGet > returned ");
        _ser.print(i);
        _ser.print(" chars from offset ");
        _ser.print(theOffset);
        _ser.print(", cfgToken='");
        _ser.print(fileWork->token);
        _ser.print("'.", true);
        */
        
        //
        // return with number of characters in string
        myTokenLen = i;
        return (i);
    } 
    else 
    {
        // error - exceeded max output buffer length
        setError(-35);
        return (-35);
    }
}

byte LD_STRM_XBEE::tokenGetSep(int theOffset) 
// uses the internal offset for starting position if TheOffset = 999
{
    byte i;

    setCurrFunction(76);
    myOC = 0;
    
    if (theOffset == 999) { theOffset = myNextOffset; }

    // initialise output buffer (token) with nulls
    for (i = 0; i < MyMaxToken; i++) 
    {
        myToken[i] = MyNull;
    }
    
    // copy chars up to the separator
    i = 0;
    while ( myBuff[(theOffset + i)] != DgSep && ((theOffset + i) < (myBuffLen + 1)) && (i < MyMaxToken) ) 
    {
        myToken[i] = myBuff[(theOffset + i)];
        i++;
    }

    // set the offset to start on the next call
    myNextOffset = (theOffset + i + 2);  // the reason we add 2 is because the Sep is a string so we need to allow for Sep + \0
    // now, make sure the token is a string
    if (myToken[(i - 1)] != MyNull) 
    {
        myToken[i] = MyNull;
        i++; // add the extra char for the \0
    }
    myTokenLen = i;
    return(i);
}
