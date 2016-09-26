#include <LD_COM.h>

/*
    LD_COM(int theRxPort, int theTxPort);
    ~LD_COM();
    void commInit(long thePortSpeed);
    byte outputInit(char *theBuffer, byte theMaxIX);
    byte outputBuild(char *theSource, char *theBuff, byte theLen);                          // commOutputBld
    byte outputFill(char *theSource, char *theBuff, byte theBuffOffset, byte theLength);
    byte outputSend(char *theBuff, char theType, byte theLength);                           // commsBufferSend
    void outputSendChar(char theChar);                                  // poss internal    // commsCharSend
    int inputAvailable();
    byte inputRecv(boolean discard = false);                                                // commsBufferGet
    byte inputLen();
    byte tokenGetLen(int theLength, int theOffset = 999);                                   // buffCharsGetLen
    byte tokenGetSep(int theOffset = 999);                                                  // buffCharsGetSep
    char tokenChar(byte theIX);
    int tokenInt();
    String tokenString();
    byte tokenLen();
    char dgType();
    byte MaxToken();
    byte MaxBuff();
*/

const byte DeviceComms = 4;

//SoftwareSerial xBee();

char myBuff[65];          // Area to read I/O chars into from a device
byte myBuffLen;           // The number of chars in the buffer
char myToken[32];         // An area to extract information from a buffer
byte myTokenLen;          // The number of chars in token (excluding MyNull)
byte myNextOffset;        // The offset of the next byte to read from the buffer
byte myOutputMax = 0;
byte myOutputIX = 0;
byte myOC = 0;
char myCommType = '*';

const byte MyMaxToken = 31;     // the maximum token IX
const byte MyMaxBuff = 64;      // the maximum buffer IX

//  Constructor
LD_COM::LD_COM(int theRxPort, int theTxPort)
    : xBee(theRxPort, theTxPort)
{
    setDevice(DeviceComms);           // See other devices
    setCurrFunction(1);
}

//  Destructor
LD_COM::~LD_COM()
{
    setCurrFunction(2);
    /* nothing to do */
}

void LD_COM::commInit(long thePortSpeed)
{
    setCurrFunction(3);
    //xBee = SoftwareSerial xBee(theRxPort, theTxPort);
    xBee.begin(thePortSpeed);
    setActive(true);
}

byte LD_COM::inputLen()
{
    setCurrFunction(4);
    return(myBuffLen);
}

char LD_COM::tokenChar(byte theIX)
{
    setCurrFunction(5);
    return(myToken[theIX]);
}

int LD_COM::tokenInt()
{
    setCurrFunction(6);
    return((String(myToken).toInt()));
}

String LD_COM::tokenString()
{   
    setCurrFunction(7);
    String s = "";
    s.concat(myToken);
    return(s);
}

byte LD_COM::tokenLen()
{
    setCurrFunction(8);
    return(myTokenLen);
}

char LD_COM::dgType()
{
    setCurrFunction(9);
    return(myCommType);
}

byte LD_COM::MaxBuff()
{
    setCurrFunction(10);
    return(MyMaxBuff);
}

byte LD_COM::MaxToken()
{
    setCurrFunction(11);
    return(MyMaxToken);
}

byte LD_COM::outputInit(char *theBuffer, byte theMaxIX)
// also initialises the 
// returns the length of the buffer as confirmation, or an error (<0)
{
    int i;
    setCurrFunction(12);

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

byte LD_COM::outputFill(char *theSource, char *theBuff, byte theBuffOffset, byte theLength)
// This does a straight copy of the theSource to the output buffer after some checking
{
    byte i;
    setCurrFunction(13);

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
            return(i + 1);
        }

    } 
    else 
    {
        return(setError(-16));
    }
}

byte LD_COM::outputBuild(char *theSource, char *theBuff, byte theLen)
// This does a copy of the source to the output buffer, but also maintains an index of where the next available 
// byte is located in the buffer - returns the number of bytes copied
{
    setCurrFunction(14);
    myOC = 0;
    // go fill the output buffer after checking it will not overflow
    myOC = outputFill(theSource, theBuff, myOutputIX, theLen);  // returns an error (<0) or the number of bytes copied
    //memcpy(&myBufferOutput[myOutputIx], &myWorkValue, theLen);
    // of successful, increment the index by the bytes copied
    if (myOC > 0) { myOutputIX += myOC; };
    return(myOC);
}

byte LD_COM::outputSend(char *theBuff, char theType, byte theLength)
//  theLength is the length of the contents in bufferOutput, not the entire length of the datagram.  We will calculate the latter.
{
    int i;
    int j = 0; // initially used to calculate the number of values in bufferOutput, but later re-used to accumulate the number of chars sent.
    
    //if (_logComms) { _ser.print("outputSend: '"); }
    
    char lenF[4];
    
    setCurrFunction(15);
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

void LD_COM::outputSendChar(char theChar) 
{
    setCurrFunction(16);
    myOC = 0;
    xBee.write(theChar);
    //if (_logComms) { _ser.print(theChar); }
}

int LD_COM::inputAvailable()
{
    setCurrFunction(16);
    myOC = 0;
    return(xBee.available());
}

byte LD_COM::inputRecv(boolean theDiscard) 
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
    String strVal = StrNull;
    char bufferInput[65];   // only used by this routine - the output is placed in myBuff

    setCurrFunction(17);
    myOC = 0;
    print("!", true);

    for (i = 0; i < MyMaxBuff; i++) 
    {
        bufferInput[i] = MyNull;
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
                print(".", false);
                //delay(100);
                commChar = xBee.read();                                              // Receive a single character from the software serial port
                print(commChar, false);
                /*
                if (_logComms) {
                _ser.print(_commChar);
                if (!foundStart || discard) { _ser.print('#'); }
                }
                */
                if (!theDiscard && foundStart && commChar != DgEnd) 
                {
                    //_commBufferInput.concat(_commChar);                                // Add the received character to the receive buffer ONLY if we have found the start of the dialog
                    bufferInput[i] = commChar;
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
        print(" ",true);
        
        if (i < MyMaxBuff) 
        {
            if (!theDiscard) 
            {
                // dump the buffer to a char array to work on
                // load the first char (Type) to _commType
                // Get the length of the structure and load it into buffLen
                for (o = 0; o < 4; o++) 
                {
                    valueF[o] = bufferInput[o];
                }
                strVal.concat(valueF);
                myBuffLen = strVal.toInt(); // this is the length WITHOUT the overhead, but with the type!
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
                    myCommType = bufferInput[4];
                    // Now load the remainder of the input buffer into the WORK_IO.buff array
                    for (o = 0; o < myBuffLen; o++)          // note that buffLen = the associated structure ONLY, not the initial overhead (4 char length + 1 char type)
                    {
                        myBuff[o] = bufferInput[(o + 5)];
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

byte LD_COM::tokenGetLen(int theLength, int theOffset) 
{

    byte i;

    setCurrFunction(18);
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
        return (i);
    } 
    else 
    {
        // error - exceeded max output buffer length
        setError(-35);
        return (-35);
    }
}

byte LD_COM::tokenGetSep(int theOffset) 
// uses the internal offset for starting position if TheOffset = 999
{
    byte i;

    setCurrFunction(19);
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

    /*
    float xh, yh, ayf, axf;
    int var_compass;
    ayf=ay/57.0;//Convert to rad
    axf=ax/57.0;//Convert to rad
    xh=cx*cos(ayf)+cy*sin(ayf)*sin(axf)-cz*cos(axf)*sin(ayf);
    yh=cy*cos(axf)+cz*sin(axf);
    var_compass=atan2((double)yh,(double)xh) * (180 / PI) -90; // angle in degrees
    if (var_compass>0){var_compass=var_compass-360;}
    var_compass=360+var_compass;
    return (var_compass);
    */

    return(i);
}
