#include "my_printf.h"
#include "function.h"
#include "led.h"
#include "lcd.h"
#include "uart.h"
#include "touchscreen.h"
#include "key.h"
#include "nand_flash.h"
#include "nor_flash.h"
#include "timer.h"
#include "at24cxx.h"
#include "i2c_controller.h"



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
				TestAt24cxx();
				break;
			case TEST_SPI:
				TestSPI();
				break;
			case TEST_IDR:
			case TEST_DHT1:
			case TEST_DS18:
			case TEST_0038T:
			case TEST_NOR:
				StopTimer();
				TestNorFlash();
				StartTimer();
				break;
			case TEST_NAND:
				StopTimer();
				TestNandFlash();
				StartTimer();
				break;
			default:
				break;
		
		}

	}
	return 0;
}



