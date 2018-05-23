#include "function.h"
#include "init.h"
#include "s3c2440_soc.h"




void delay(volatile int d)
{
	while (d--);
}

void LED_test(void)
{
	int val = 0;  /* val: 0b000, 0b111 */
	int tmp;
	led_init();

	/* —≠ª∑µ„¡¡ */
	while (1)
	{
		tmp = ~val;
		tmp &= 7;
		GPFDAT &= ~(7<<4);
		GPFDAT |= (tmp<<4);
		delay(100000);
		val++;
		if (val == 8)
			val =0;
		
	}	
}


