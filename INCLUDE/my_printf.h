
#ifndef _MY_PRINTF_H
#define _MY_PRINTF_H

#include "uart.h"
#define  __out_putchar  PutChar


#define  MAX_NUMBER_BYTES  64

int printf(const char *fmt, ...);
void printHex(unsigned int val);
void print1(void);



#endif /* _MY_PRINTF_H */

