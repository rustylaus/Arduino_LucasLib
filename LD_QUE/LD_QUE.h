//
//  QUEUE SUB_SYSTEM HEADER FILE
//
//  (c) Lucas Design & RustyL Ventures P/L, 2016.  All rights reserved.
//

#ifndef LD_QUE_H
#define LD_QUE_H

#include <Arduino.h>
#include "LD_SER.h"
#include "Adafruit_FRAM_SPI.h"

typedef struct {
  char type;
  byte len;
  unsigned int addr;
} DEQUEUE_ITEM;

class LD_QUE_H : public LD_SER
{
    
public:
    LD_QUE();
    ~LD_QUE();
    byte initQ(int RAMport, boolean fullInit = false)                                        // returns bad blocks found in FRAM
    void saveQ();
    byte currItemCount();
    byte writeQitem(char *theData, byte theLength, char theType);
    byte readQaddr(DEQUEUE_ITEM *myItem);
    byte readItem(uint16_t addr, char *buff, int len);  // copies the item from the RAM address into the buffer for the 
                                                        // specified length, & returns length of item copied.
                                                        // NB: readQ returns the address in DEQUEUE_ITEM->addr                                              
}

#endif