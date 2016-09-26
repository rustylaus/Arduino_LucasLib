//
//  QUEUE SUB_SYSTEM HEADER FILE
//
//  (c) Lucas Design & RustyL Ventures P/L, 2016.  All rights reserved.
//

#ifndef LD_QUE_H
#define LD_QUE_H

#include <Arduino.h>
//#include "../LD_DEV/LD_DEV.h"
#include <LD_SER.h>
#include <Adafruit_FRAM_SPI.h>

typedef struct {
  char type;
  byte len;
  unsigned int addr;
} DEQUEUE_ITEM;

class LD_QUE : public LD_SER
{
    
public:
    LD_QUE(int RAMport);
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

private:
    Adafruit_FRAM_SPI fram;

};

#endif