#include "uart.h"
#include "my_printf.h"


void printException(unsigned int cpsr, char *str)
{
	Puts("Exception! cpsr = ");
	printHex(cpsr);
	Puts(" ");
	Puts(str);
	Puts("\n\r");
}

void printSWIVal(unsigned int *pSWI)
{
	Puts("SWI val = ");
	printHex(*pSWI & ~0xff000000);
	Puts("\n\r");
}

