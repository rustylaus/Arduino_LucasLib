#include <LD_QUE.h>

/*
    LD_QUE(String theUnit, byte theDeviceNumber, boolean serialEnabled, int RAMport);
    ~LD_QUE();
    byte initQ(boolean fullInit = false);                           // returns bad blocks found in FRAM
    byte saveQ();
    byte currItemCount();
    byte writeQitem(char *theData, byte theLength, char theType);
    byte readQaddr(DEQUEUE_ITEM *myItem);
    byte readItem(uint16_t addr, char *buff, int len);              // copies the item from the RAM address into the buffer for the 
                                                                    // specified length, & returns length of item copied.
                                                                    // NB: readQ returns the address in DEQUEUE_ITEM->addr                
    int NTwrite();                                                  // Next to write - zero relative slot number
    int NTread();                                                   // Next to read - relative slot number
    byte CountCurr();                                               // Number of items in the buffer
    byte CountCurrMax();                                            // Maximum items ever Qd in the buffer this run
    byte CountOverflow();                                           // The number of times the buffer has been full
    int CountQd();                                                  // The number of items that has been queued in the buffer
    int CountDeQd();                                                // The number of items that have been dequeued in the buffer
    byte NoSlots();                                                 // The maximum number of available slots in the queue          
*/

//  Constructor
LD_QUE::LD_QUE(byte theUnit, byte theDeviceNumber, boolean serialEnabled, int RAMport)
    : fram(RAMport)
{
    setCurrFunction(65);
    deviceInit(theUnit, theDeviceNumber, serialEnabled);
    myRAMport = RAMport;
    setEnabled(true);
}

//  Destructor
LD_QUE::~LD_QUE()
{
    setCurrFunction(66);
    /* nothing to do */
}

byte LD_QUE::NTwrite()
{
    return(myNTwrite);
}

byte LD_QUE::NTread()
{
    return(myNTread);
}

byte LD_QUE::CountCurr()
{
    return(myCountCurr);
}

byte LD_QUE::CountCurrMax()
{
    return(myCountCurrMax);
}

byte LD_QUE::CountOverflow()
{
    return(myCountOverflow);
}

int LD_QUE::CountQd()
{
    return(myCountQd);
}

int LD_QUE::CountDeQd()
{
    return(myCountDeQd);
}

byte LD_QUE::NoSlots()
{
    return(MyNoSlots);
}

//  Queue initialisation - returns the number of bad blocks
byte LD_QUE::initQ(boolean fullInit)
{

    setCurrFunction(67);

    if (fram.begin()) 
    {  
        //_ser.print("FRAM found and initialised.", true);
        setActive(true);
    } 
    else 
    {
        //_ser.print("No SPI FRAM found ... check your connections\r\n", true);
        setError(-11);
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
        print("FRAM full init... ");
        uint16_t addrToUse = 0;
        char myValue[4];
        char buff[5];

        String strVal;
        byte isOK = 0;
        boolean initBuffer = false;

        char initBuffOut[64];
        char initBuffIn[64];
        const byte InitBuffMax = 63;
    
        for (j = 0; j < InitBuffMax; j++) 
        {
            initBuffOut[j] = MyNull;
        }
        
        buff[0] = 'N';
        buff[4] = 'N';
        //print("Writing... ");
        for (i = 0; i < MyNoSlots; i++) 
        { 
            addrToUse = (i * MyBlockSize);
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
        /*
        print("Done [");
        print(i);
        print("]... Comparing...", true);
        */
        for (i = 0; i < MyNoSlots; i++) 
        {
            addrToUse = (i * MyBlockSize);
            myValue[0] = fram.read8(addrToUse);
            if (myValue[0] == 'N') 
            {
                addrToUse ++;
                readItem(addrToUse, &myValue[0], 3);
                myValue[3] = MyNull;
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
                        readItem(addrToUse, &initBuffIn[0], (InitBuffMax + 1));
                        for (j = 0; j < (InitBuffMax + 1); j++) 
                        {
                            if (initBuffIn[j] != initBuffOut[j]) { isOK = j + 4; }
                        }
                    } 
                    else 
                    {
                        isOK = 3;
                    }  
                } 
                else 
                {
                    isOK = 2;
                }
            } 
            else 
            {
                isOK = 1;
            }
            if (isOK != 0) 
            {
                BadBlockCount++;
                /*
                print("FRAM Err=");
                print(isOK);
                print(",block=");
                print(i);
                print(",badblocks=");
                print(BadBlockCount);
                print(".",true);
                */
                buff[0] = 'X';
                fram.writeEnable(true);
                fram.write((i * MyBlockSize), (uint8_t *)buff, 1);
                fram.writeEnable(false);
                isOK = 0;
            }      
        }
    }
    saveQ();
    print("done. Badblocks=");
    print(BadBlockCount, true);
    return(BadBlockCount);
}

//  Saves the Queue header to non-volatile storage
byte LD_QUE::saveQ()
{   
    setCurrFunction(68);
    uint16_t addr = MyHeaderSlot;

    if (isActive()) 
    {
        char myValue[5] = "QHDR";
        fram.writeEnable(true);
        fram.write(addr, (uint8_t *)myValue, 4);
        fram.writeEnable(false);
        addr += 4;
        sprintf(myValue, Fmt3u, MyNoSlots);
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
    } 
    else 
    {
        return(setError(-15));
    }
}

//->Queing>Prepares an empty slot in the queue for the item to be loaded - length of item written is returned (or an error)
byte LD_QUE::writeQitem(char *theData, byte theLength, char theType)
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

    setCurrFunction(69);
    byte indexToUse = 0;
    unsigned int addrToUse = 0;
    char inUse[2];
    inUse[0] = 'X';
    char myValue[4];
    char myType[2];
    myType[0] = theType;

    if (isActive()) 
    {
        // check the length parm
        if (theLength < 1 || (theLength + RAMoverhead) > MyBlockSize) { return(setError(-21)); }

        // if ixWriteNext = ixReadNext AND count of items not zero, then Q is full - increment countQfull & exit with -22
        if (myNTwrite == myNTread && myCountCurr > 0) 
        {
            myCountOverflow++;
            return(setError(-20));
        }
            
        // get the index to use
        indexToUse = myNTwrite;
        // check if the block is in use and return an error if it is
        addrToUse = (MyDgStartSlot * MyBlockSize) + (indexToUse * MyBlockSize) - 1 ;
        noInterrupts();
        inUse[0] = fram.read8(addrToUse);
        // only do the following loop if In-Use is an 'X' (bad block)... we will try to read past it - if it is in Use (a 'Y') fall through to return
        while (inUse[0] == 'X') 
        {
            // OK, lets try the next block
            myNTwrite ++;
            // if we have wrapped past the end, go to the first block
            if (myNTwrite > MyDgEndSlot) { myNTwrite = MyDgStartSlot; }
            // get the index to use
            indexToUse = myNTwrite;
            // get the In-Use char
            addrToUse = (indexToUse * MyBlockSize) - 1;
            inUse[0] = fram.read8(addrToUse);
            // now check again
        }
        interrupts();
        // if we dropped through because it is in use, we cannot proceed.
        if (inUse[0] == 'Y') { return(setError(-21)); }
        
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
        if (myNTwrite > MyDgEndSlot) { myNTwrite = MyDgStartSlot; }
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
        // now write the item to the FRAM
        char myBuff[65];        // create a temp buffer
        int i;                  //  copy the data into my temp buffer
        for (i = 0; i < theLength; i++)
        {
            myBuff[i] = *(theData + i);
        }
        // write my temp buffer to the FRAM
        noInterrupts();
        fram.writeEnable(true);
        fram.write(addrToUse, (uint8_t *)myBuff, theLength);
        fram.writeEnable(false);
        interrupts();
        return(theLength);
    } 
    else 
    {
        return(setError(-22));
    }        
}

//  
byte LD_QUE::readQaddr(DEQUEUE_ITEM *myItem)
{   
    setCurrFunction(70);

    if (isActive()) 
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
            myItem->len = -25;
            return(setError(-25));
        }
        
        indexToUse = myNTread;  // check if the block is in use and return an error if it is
        addrToUse =  (MyDgStartSlot * MyBlockSize) + (indexToUse * MyBlockSize) - 1;
        noInterrupts();
        inUse[0] = fram.read8(addrToUse);

        // check if it a bad block, and try to read past it if it is..
        while (inUse[0] == 'X') 
        {
            myNTread ++;
            if (myNTread > MyDgEndSlot) { myNTread = MyDgStartSlot; }
            indexToUse = myNTread;  // check if the block is in use and return an error if it is
            addrToUse = (indexToUse * MyBlockSize) - 1;
            inUse[0] = fram.read8(addrToUse);
            // now check it again
        }
        interrupts();

        // if it isnt a bad block, and the block is not in use, we cannot continue
        if (inUse[0] == 'N') 
        {
            myItem->len = -26;
            return(setError(-26));
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
        readItem(addrToUse, &myValue[0], 3);
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
        if (myNTread > MyDgEndSlot) { myNTread = MyDgStartSlot; }

        saveQ();
        return(myItem->len);  // return the type of the item, and the index to that type's queue so the caller can retrieve the item
    } 
    else 
    {
        return(setError(-27));
    }
}

//  
byte LD_QUE::readItem(uint16_t addr, char *buff, int len)
{
    setCurrFunction(71);
    if (isActive())
    {   
        int i;
        for (i = 0; i < len; i++)
        {
            *(buff + i) = fram.read8(addr + i);
        }
        return(i + 1);
    } 
    else 
    {
        return(setError(-30));
    }
}

//  Returns the number of items currently queued
byte LD_QUE::currItemCount()
{
    setCurrFunction(72);
    if (isActive())
    {   
        return(myCountCurr);
    } 
    else 
    {
        return(setError(-35));
    }
}