
#ifndef _MY_PRINTF_H
#define _MY_PRINTF_H

#include "uart.h"
#define  __out_putchar  putchar


#define  MAX_NUMBER_BYTES  64

int printf(const char *fmt, ...);
void printHex(unsigned int val);


#endif /* _MY_PRINTF_H */

