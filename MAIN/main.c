#include "my_printf.h"
#include "init.h"
#include "function.h"
#include "uart.h"



int main(void)
{
	uart0_init();
	printf("\n\rsuccessfully!!!\n\r");
	
	LED_test();	
	
	return 0;
}



