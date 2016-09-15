#include "C:\Users\Russell\SkyDrive\_Dev\LD_Libraries\LD_COM\LD_COM.h"

/*
    LD_COM();
    ~LD_COM();
    void (int theRxPort, int theTxPort, long thePortSpeed)
    byte outputBuild(byte theLen);                                      // commOutputBld
    int outputSend(char theType, int theLength);                         // commsBufferSend
    void outputSendChar(char theChar);               // poss internal    // commsCharSend
    int inputRecv(boolean discard = false);             // commsBufferGet
    int tokenGetLen(int theOffset, int theLength);     // buffCharsGetLen
    int tokenGetSep(int theOffset);                    // buffCharsGetSep
    void setBufferOutput(byte theIX, char theChar);
    char bufferOutput(byte theIX);
    char token(byte theIX);
    byte tokenLen();
    char dgType();
    
    //  The following is to be confirmed... may be duplicate to the above

    // if WORK_IO is contained in class - we need functions to set/get variables
    // what about the _commbuffers?

    char type();
    char input(byte theOffset);
    char inputSet(byte theOffset, char theChar, byte theLen = 1);
    char output(byte theOffset);
    char outputSet(byte theOffset, char theChar, byte theLen = 1);
    String token();
    nextOffset


*/

const byte DeviceComms = 4;

SoftwareSerial xBee;

char myBuff[65];          // Area to read I/O chars into from a device
byte myBuffLen;           // The number of chars in the buffer
char myToken[32];         // An area to extract information from a buffer
byte myTokenLen;          // The number of chars in token (excluding MyNull)
byte myNextOffset;        // The offset of the next byte to read from the buffer

char myWorkValue[33];     // Need to check this is big enough for GPS values

char myBufferOutput[65];
byte myOutputIX = 0;
byte myOC = 0;
char myCommType = '*';

const byte MaxToken = 31;
const byte MaxBuff = 64;


//  Constructor
LD_COM::LD_COM()
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

void LD_COM::initComm(int theRxPort, int theTxPort, long thePortSpeed)
{

    setCurrFunction(3);

    SoftwareSerial xBee(theRxPort, theTxPort);
    xBee.begin(thePortSpeed);
    setActive(true);

}

void LD_COM::setBufferOutput(byte theIX, char theChar)
{
    myBufferOutput[theIX] = theChar;
}

char LD_COM::bufferOutput(byte theIX)
{
    return(myBufferOutput[theIX]);
}

byte LD_COM::buffInputLen()
{
    return(myBuffLen);
}

char LD_COM::token(byte theIX)
{
    return(myToken[theIX]);
}

byte LD_COM::tokenLen()
{
    return(myTokenLen);
}

byte LD_COM::dgType()
{
    return(myCommType);
}

byte LD_COM::outputBuild(byte theLen)                               // returns the number of bytes copied
{
    setCurrFunction(4);
    myOC = 0;
    // check we do not overflow the buffer
    memcpy(&myBufferOutput[myOutputIx], &myWorkValue, theLen);
    myOutputIx += theLen;
    return(theLen);
}

int LD_COM::outputSend(char theType, int theLength)
//  theLength is the length of the contents in bufferOutput, not the entire length of the datagram.  We will calculate the latter.
{
    int i;
    int j = 0; // initially used to calculate the number of values in bufferOutput, but later re-used to accumulate the number of chars sent.
    
    //if (_logComms) { _ser.print("outputSend: '"); }
    
    char lenF[4];
    
    setCurrFunction(5);
    myOC = 0;

    if (isActive())
    {
        // find out how many fields there are by summing the number of NULLs in bufferOutput
        for (i=0; i < theLength; i++) 
        {
            if (myBufferOutput[i] == '\0') {
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
            outputSendChar(myBufferOutput[i]);
            if (myBufferOutput[i] == '\0') 
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
    } else
    {
        // what happens if not active?
        myOC = -60
        setError(myOC,false);
    }
    return(myOC);
}

void LD_COM::outputSendChar(char theChar) 
{
    setCurrFunction(6);
    myOC = 0;
    xBee.write(theChar);
    //if (_logComms) { _ser.print(theChar); }
}

int LD_COM::inputRecv(boolean theDiscard = false) 
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
    char bufferInput[65];   // this is only used by this routine.

    setCurrFunction(7);
    myOC = 0;
    _ser.print("!", true);

    for (i = 0; i < MaxBuff; i++) 
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
        while (commChar != DgEnd && i < MaxBuff)        // We could loop for-ever here waiting for the end sentinel - may need a circuit breaker in case of comms error
        {                                            
            c = xBee.available();
            //_ser.print(c);
            //delay(2000);
            if (c > 0) 
            {
                _ser.print(".", false);
                //delay(100);
                commChar = xBee.read();                                              // Receive a single character from the software serial port
                _ser.print(commChar, false);
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
            } else 
            {
                // there were no chars - if this is a discard scenario - then that is fine - we exit.
                if (theDiscard) { commChar = DgEnd; }
            }
        }
        //if (_logComms) { _ser.print("'."); }
        //_ser.print("<");
        //_ser.print(i, true);
        _ser.print(" ",true);
        
        if (i < MaxBuff) 
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
                    ackResInvSend(DgTypeInvalid);
                    myOC = -72;        // bad length
                } else 
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
            } else 
            {
                myOC = 0;
            } 
        } else 
        {
            //*ioWork->buffLen = 0;
            myOC = -71;            // buffer overflow - no full message can be greater than 128 chars
        }
        myNextOffset = 0;
        //interrupts();
    } else {
        myOC = -70; 
    }
    if (myOC < 0) { setError(myOC, false); }
    return(myOC);
}

int LD_COM::tokenGetLen(int theOffset, int theLength) 
{

    byte i;

    setCurrFunction(8);
    myOC = 0;

    // check we are not asking for something beyond the end of the buffer
    if ((theLength < (MaxToken)) && ((theOffset + theLength) < (myBuffLen + 1))) 
    {
        // initialise output buffer (token) with nulls
        for (i=0; i<(MaxToken); i++) 
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
    } else 
    {
        // error - exceeded max output buffer length
        setError(-73, false);
        return (-73);
    }
}

int LD_COM::tokenGetSep(int theOffset) 
{
    byte i;

    setCurrFunction(9);
    myOC = 0;
    
    // initialise output buffer (token) with nulls
    for (i=0; i<(MaxToken); i++) 
    {
        myToken[i] = MyNull;
    }
    
    // copy chars up to the separator
    i=0;
    while ( myBuff[(theOffset + i)] != DgSep && ((theOffset + i) < (myBuffLen + 1)) && (i < MaxToken) ) 
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
