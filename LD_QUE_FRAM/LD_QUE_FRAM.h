//
//  FRAM QUEUE SUB_SYSTEM HEADER FILE
//
//  (c) Lucas Design & RustyL Ventures P/L, 2016.  All rights reserved.
//

#ifndef LD_QUE_FRAM_H
#define LD_QUE_FRAM_H

#include <Arduino.h>
#include <LD_BASE.h>
#include <Adafruit_FRAM_SPI.h>

typedef struct 
{
    char type;
    int len;
    unsigned int addr;
} DEQUEUE_ITEM;

const byte RAMoverhead = 5;

const int MyHeaderSlot = 0;
const int MyDgStartSlot = 1;
const int MyDgEndSlot = 101;
const byte MyNoSlots = 100; 
const byte MyBlockSize = 69;

class LD_QUE_FRAM : public LD_BASE
{

    byte myNTwrite = 0;              // Next to write - zero relative slot number
    byte myNTread = 0;               // Next to read - relative slot number
    byte myCountCurr = 0;           // Number of items in the buffer
    byte myCountCurrMax = 0;        // Maximum items ever Qd in the buffer this run
    byte myCountOverflow = 0;       // The number of times the buffer has been full
    int myCountQd = 0;              // The number of items that has been queued in the buffer
    int myCountDeQd = 0;            // The number of items that has been de-queued from the buffer
    int myRAMport = 0;

public:
    LD_QUE_FRAM(byte theUnit, byte theDeviceNumber, boolean serialEnabled, int RAMport);
    ~LD_QUE_FRAM();
    byte queueBegin(boolean fullInit = false);                           // returns bad blocks found in FRAM
    byte queueSave();
    byte queueCountCurrItem();
    byte queueItemWrite(char *theData, byte theLength, char theType);
    byte queueItemNextAddr(DEQUEUE_ITEM *myItem);
    byte queueItemRead(uint16_t addr, char *buff, byte len);              // copies the item from the RAM address into the buffer for the 
                                                                    // specified length, & returns length of item copied.
                                                                    // NB: readQ returns the address in DEQUEUE_ITEM->addr                
    byte NTwrite();                                                  // Next to write - zero relative slot number
    byte NTread();                                                   // Next to read - relative slot number
    byte CountCurr();                                               // Number of items in the buffer
    byte CountCurrMax();                                            // Maximum items ever Qd in the buffer this run
    byte CountOverflow();                                           // The number of times the buffer has been full
    int CountQd();                                                  // The number of items that has been queued in the buffer
    int CountDeQd();                                                // The number of items that have been dequeued in the buffer
    byte NoSlots();                                                 // The maximum number of available slots in the queue                                            

private:
    Adafruit_FRAM_SPI fram;

};

#endif