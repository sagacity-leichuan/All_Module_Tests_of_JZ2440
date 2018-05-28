#include "my_printf.h"
#include "function.h"
#include "led.h"
#include "lcd.h"
#include "uart.h"
#include "touchscreen.h"
#include "tslib.h"



int main(void)
{
	int x,y,iPressure;
	printf("\n\r Start running! \n\r");
	
	BootInit();

	BootScreen();

	MainPage();

	while (1)
	{
		ReadTs(&x, &y, &iPressure);	
	}
	
	return 0;
}



