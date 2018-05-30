#include "my_printf.h"
#include "function.h"
#include "led.h"
#include "lcd.h"
#include "uart.h"
#include "touchscreen.h"
#include "key.h"


int main(void)
{
	int iTestItem = ORIGINAL;
	
	printf("\n\r Start running! \n\r");
	
	BootInit();

	//BootScreen();

	MainPage();

	while (1)
	{
		iTestItem = ORIGINAL;
		GetTestItem(&iTestItem);
		Delay(1000);

		switch (iTestItem)
		{
			case TEST_LED:
				TestLED();
				break;
			case TEST_KEY:
				TestKey();
				break;
			case TEST_LCD:
				TestLCD();
				break;
			case TEST_TS:
				TestTouchScreen();
				break;
			case TEST_I2C:
			case TEST_SPI:
			case TEST_IDR:
			case TEST_DHT1:
			case TEST_0038T:
			case TEST_NOR:
			case TEST_NAND:
			default:
				break;
		
		}

	}
	return 0;
}



