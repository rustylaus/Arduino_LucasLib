#include "C:\Users\Russell\SkyDrive\_Dev\LD_Libraries\LD_QUE\LD_QUE.h"

/*
    LD_QUE();
    ~LD_QUE();
    int initQ();                                         // returns bad blocks found in FRAM
    void saveQ();
    byte currItemCount();
    unsigned int writeQ(byte theLength, char theType);
    int readQ(DEQUEUE_ITEM *myItem);
    int copyItem(uint16_t addr, char *buff, int len); 
*/

const byte DeviceQueue = 3;

const byte RAMoverhead = 5;

int myNTwrite = 0;              // Next to write - zero relative slot number
int myNTread = 0;               // Next to read - relative slot number
byte myCountCurr = 0;           // Number of items in the buffer
byte myCountCurrMax = 0;        // Maximum items ever Qd in the buffer this run
byte myCountOverflow = 0;       // The number of times the buffer has been full
int myCountQd = 0;              // The number of items that has been queued in the buffer
int myCountDeQd = 0;            // The number of items that has been de-queued from the buffer

const int myHeaderSlot = 0;
const int myDgStartSlot = 1;
const int myDgEndSlot = 101;
const byte myNoSlots = 100; 
const byte myBlockSize = 69;

//  FRAM Extended memory
Adafruit_FRAM_SPI fram; //= Adafruit_FRAM_SPI(SPIportSelectFRAM); 
int myRAMport = 0;

//  Constructor
LD_QUE::LD_QUE()
{
    setDevice(DeviceQueue);
    setCurrFunction(1);
    setEnabled(true);
}

//  Destructor
LD_QUE::~LD_QUE()
{
    setCurrFunction(2);
    /* nothing to do */
}

//  Queue initialisation - returns the number of bad blocks
int LD_QUE::initQ(int RAMport, boolean fullInit = false)
{

    setCurrFunction(3);
    myRAMport = RAMport;
    fram = Adafruit_FRAM_SPI(myRAMport);

    if (fram.begin()) {  
        //_ser.print("FRAM found and initialised.", true);
        setActive(true);
    } else {
        //_ser.print("No SPI FRAM found ... check your connections\r\n", true);
        setError(-128, false);
        while (true) { ; }
    }

    myNTwrite = 0;
    myNTread = 0;
    myCountCurr = 0;
    myCountCurrMax = 0;
    myCountOverflow = 0;
    myCountQd = 0;
    myCountDeQd = 0;
    
    int i;
    int j;
    int BadBlockCount = 0;

    if (fullInit) 
    {
        uint16_t addrToUse = 0;
        char myValue[4];
        char buff[5];

        String strVal;
        boolean isOK = true;
        boolean initBuffer = false;

        char initBuffOut[64];
        char initBuffIn[64];
        const byte InitBuffMax = 63;
    
        for (j = 0; j < InitBuffMax; j++) 
        {
            initBuffOut[j] = '\0';
        }
        
        buff[0] = 'N';
        buff[4] = 'N';
        
        for (i = 0; i < myNoSlots; i++) 
        { 
            addrToUse = (i * myBlockSize);
            strVal = "";
            sprintf(myValue, Fmt3u, 0);  // format as a string "009"\0 - but only write the first 3 chars - not the null
            for (j=1; j<4; j++)
            {
                buff[j] = myValue[j-1];
            }

            fram.writeEnable(true);
            fram.write(addrToUse, (uint8_t *)buff, 5);
            fram.writeEnable(false);
            addrToUse += 5;
            fram.writeEnable(true);
            fram.write(addrToUse, (uint8_t *)initBuffOut, (InitBuffMax + 1));
            fram.writeEnable(false);
        }
        
        for (i = 0; i < myNoSlots; i++) 
        {
            addrToUse = (i * myBlockSize);
            myValue[0] = fram.read8(addrToUse);
            if (myValue[0] == 'N') 
            {
                addrToUse ++;
                copyItem(addrToUse, &myValue[0], 3);
                myValue[3] = '\0';
                strVal = StrNull;
                strVal.concat(myValue);
                j = strVal.toInt();
                if (j == 0) 
                {
                    addrToUse +=3;
                    myValue[0] = fram.read8(addrToUse);
                    if (myValue[0] == 'N') 
                    {
                        addrToUse ++;
                        copyItem(addrToUse, &initBuffIn[0], (CommBufferMax + 1));
                        for (j = 0; j < 123; j++) 
                        {
                            if (initBuffIn[j] != initBuffOut[j]) { isOK = false; }
                        }
                    } else {
                        isOK = false;
                    }  
                } else {
                    isOK = false;
                }
            } else {
                isOK = false;
            }
            if (!isOK) 
            {
                BadBlockCount++;
                buff[0] = 'X';
                fram.writeEnable(true);
                fram.write((i * myBlockSize), (uint8_t *)buff, 1);
                fram.writeEnable(false);
            }      
        }
    }
    saveQ();
    return(BadBlockCount);
}

//  Saves the Queue header to non-volatile storage
void LD_QUE::saveQ()
{   
    setCurrFunction(4);
    uint16_t addr = myHeaderSlot;

    if (isActive) 
    {
        char myValue[5] = "QHDR";
        fram.writeEnable(true);
        fram.write(addr, (uint8_t *)myValue, 4);
        fram.writeEnable(false);
        addr += 4;
        sprintf(myValue, Fmt3u, myNoSlots);
        fram.writeEnable(true);
        fram.write(addr, (uint8_t *)myValue, 3);
        fram.writeEnable(false);
        addr += 3;
        sprintf(myValue, Fmt3u, myNTwrite);
        fram.writeEnable(true);
        fram.write(addr, (uint8_t *)myValue, 3);
        fram.writeEnable(false);
        addr += 3;
        sprintf(myValue, Fmt3u, myNTread);
        fram.writeEnable(true);
        fram.write(addr, (uint8_t *)myValue, 3);
        fram.writeEnable(false);
        addr += 3;
        sprintf(myValue, Fmt3u, myCountCurr);
        fram.writeEnable(true);
        fram.write(addr, (uint8_t *)myValue, 3);
        fram.writeEnable(false);
        addr += 3;
        sprintf(myValue, Fmt4u, myCountQd);
        fram.writeEnable(true);
        fram.write(addr, (uint8_t *)myValue, 4);
        fram.writeEnable(false);
        addr += 4;
        sprintf(myValue, Fmt4u, myCountDeQd);
        fram.writeEnable(true);
        fram.write(addr, (uint8_t *)myValue, 4);
        fram.writeEnable(false);
        addr += 4;
        sprintf(myValue, Fmt4u, myCountOverflow);
        fram.writeEnable(true);
        fram.write(addr, (uint8_t *)myValue, 4);
        fram.writeEnable(false);
        addr += 4;
        sprintf(myValue, Fmt3u, myCountCurrMax);
        fram.writeEnable(true);
        fram.write(addr, (uint8_t *)myValue, 3);
        fram.writeEnable(false);
        addr += 4;
        myValue[0] = '<'; myValue[1] = 'E'; myValue[2] = 'N'; myValue[3] = 'D';
        fram.writeEnable(true);
        fram.write(addr, (uint8_t *)myValue, 4);
        fram.writeEnable(false);
    } else {
        return(setError(-10))
    }
}

//->Queing>Prepares an empty slot in the queue for the item to be loaded - the FRAM address to write to is returned (or an error)
unsigned int  LD_QUE::writeQ(byte theLength, char theType)
{   
    // The format of the FRAM block is as follows:
    //     U999T<struct>
    // where:
    //   U = the Block-In-Use flag - either Y (yes, in use), N (available) or X (bad block).
    //   999 = ONLY the length of the structure following the Type (excluding the header overhead "Y999T" = 5 chars >> Note no NULL after length)
    //   T = the datagram type (as supplied to this function)
    //   <struct> = the structure being written

    // Do we need to extend this to allow for good and bad blocks?
    
    // theLength is the length of the structure to be written - we need to add the overhead to it.  The overhead is currently 6 (SRAMoverhead).

    setCurrFunction(5);
    byte indexToUse = 0;
    unsigned int addrToUse = 0;
    char inUse[2];
    inUse[0] = 'X';
    char myValue[4];
    char myType[2];
    myType[0] = theType;

    if (isActive) 
    {
        // check the length parm
        if (theLength < 1 || (theLength + RAMoverhead) > myBlockSize) { (return(setError(-21)); }

        // if ixWriteNext = ixReadNext AND count of items not zero, then Q is full - increment countQfull & exit with -22
        if (myNTwrite == myNTread && myCountCurr > 0) 
        {
            myCountOverflow++;
            return(setError(-22));
        }
            
        // get the index to use
        indexToUse = myNTwrite;
        // check if the block is in use and return an error if it is
        addrToUse = (myDgStartSlot * myBlockSize) + (indexToUse * myBlockSize) - 1 ;
        noInterrupts();
        inUse[0] = fram.read8(addrToUse);
        // only do the following loop if In-Use is an 'X' (bad block)... we will try to read past it - if it is in Use (a 'Y') fall through to return
        while (inUse[0] == 'X') 
        {
            // OK, lets try the next block
            myNTwrite ++;
            // if we have wrapped past the end, go to the first block
            if (myNTwrite > myDgEndSlot) { myNTwrite = myDgStartSlot; }
            // get the index to use
            indexToUse = myNTwrite;
            // get the In-Use char
            addrToUse = (indexToUse * myBlockSize) - 1;
            inUse[0] = fram.read8(addrToUse);
            // now check again
        }
        interrupts();
        // if we dropped through because it is in use, we cannot proceed.
        if (inUse[0] == 'Y') { return(setError(-23)); }
        
        // OK, all is in order - we are set to go...
        
        // increment the count of items
        myCountCurr ++;
        // increment the number of items that have been Qd
        myCountQd ++;
        // if count of items > maxQd, the update maxQd
        if (myCountQd > myCountCurrMax) { myCountCurrMax = myCountQd; }
        // increment the ixWriteNext    
        myNTwrite ++;
        // if ixWriteNext > ixMax then ixWriteNext = Start Slot;
        if (myNTwrite > myDgEndSlot) { myNTwrite = myDgStartSlot; }
        /* DEBUG
        print("queueWrite:: Addr = ");
        print(addrToUse);
        print(", type = ");
        print(myType[0],true);
        */
        // set block in use
        noInterrupts();
        inUse[0] = 'Y';
        fram.writeEnable(true);
        fram.write(addrToUse, (uint8_t *)inUse, 1);
        fram.writeEnable(false);
        addrToUse++;
        // write the stream length + '\0' at at an incremented address
        sprintf(myValue, Fmt3u, theLength);
        fram.writeEnable(true);
        fram.write(addrToUse, (uint8_t *)myValue, 3);       // we are not reading or writing the NULL - only the three numeric chars
        fram.writeEnable(false);
        addrToUse +=3;
        // write the type at an incremented address
        fram.writeEnable(true);
        fram.write(addrToUse, (uint8_t *)myType, 1);
        fram.writeEnable(false);
        interrupts();
        addrToUse++;
        // return calculated address as offset with the RAM - use this address to start writing the structure
        saveQ();
        return(addrToUse);
    } else {
        return(setError(-10))
    }        
}

//  
int LD_QUE::readQ(DEQUEUE_ITEM *myItem)
{   
    setCurrFunction(6);

    if (isActive) 
    {
        byte indexToUse;
        char typeToUse;
        unsigned int addrToUse = 0;
        char inUse[2];
        char myValue[4];
        String strVal = StrNull;

        myItem->type = MyNull;
        myItem->len = 0;  // also used to return an outcome/error - if negative, then it is an error
        myItem->addr = 0;
        
        // if ixReadNext = ixWriteNext AND count is zero, the Q is empty - return with -31
        if (myNTread == myNTwrite && myCountCurr == 0) 
        {
            myItem->len = -31;
            return(setError(-31));
        }
        
        indexToUse = myNTread;  // check if the block is in use and return an error if it is
        addrToUse =  (myDgStartSlot * myBlockSize) + (indexToUse * myBlockSize) - 1;
        noInterrupts();
        inUse[0] = fram.read8(addrToUse);
        // check if it a bad block, and try to read past it if it is..
        while (inUse[0] == 'X') 
        {
            myNTread ++;
            if (myNTread > myDgEndSlot) { myNTread = myDgStartSlot; }
            indexToUse = myNTread;  // check if the block is in use and return an error if it is
            addrToUse = (indexToUse * myBlockSize) - 1;
            inUse[0] = fram.read8(addrToUse);
            // now check it again
        }
        interrupts();
        // if it isnt a bad block, and the block is not in use, we cannot continue
        if (inUse[0] == 'N') 
        {
            myItem->len = -32;
            return(setError(-32));
        }

        // must be 'Y'
        // OK, all is in order - we have found the first block that is in use... we are set to go...
        // set block NOT in use
        noInterrupts();
        inUse[0] = 'N';
        fram.writeEnable(true);
        fram.write(addrToUse, (uint8_t *)inUse, 1);
        fram.writeEnable(false);
        addrToUse++;

        // retrieve and store the stream length
        copyItem(addrToUse, &myValue[0], 3);
        myValue[4] = MyNull;
        strVal = StrNull;
        strVal.concat(myValue);
        myItem->len = strVal.toInt();
        addrToUse +=3;
        
        // retrieve and store the type at an incremented address
        myItem->type = fram.read8(addrToUse);
        interrupts();
        // increment addrToUse and store it in the return data
        myItem->addr = addrToUse + 1;

        // decrement the count of items
        myCountCurr --;
        // increment the number of items that have been De-Queued
        myCountDeQd ++;
        // increment the ixReadNext    
        myNTread ++;
        // if ixReadNext > ixMax then ixReadNext = 0;
        if (myNTread > myDgEndSlot) { myNTread = myDgStartSlot; }

        saveQ();
        return(myItem->len);  // return the type of the item, and the index to that type's queue so the caller can retrieve the item
    } else {
        return(setError(-10))
    }
}

//  
int LD_QUE::copyItem(uint16_t addr, char *buff, int len)
{
    setCurrFunction(7);
    if (isActive)
    {   
        int i;
        for (i = 0; i < len; i++)
        {
            *(buff + i) = fram.read8(addr + i);
        }
        return(i + 1);
    } else {
        return(setError(-10))
    }
}

//  Returns the number of items currently queued
byte LD_QUE::currItemCount()
{
    setCurrFunction(8);
    if (isActive)
    {   
       return(myCountCurr);
    } else {
        return(setError(-10))
    }
}