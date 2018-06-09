#include "s3c2440_soc.h"

void InitADC(int channel)
{
	/* [15] : ECFLG,  1 = End of A/D conversion
	 * [14] : PRSCEN, 1 = A/D converter prescaler enable
	 * [13:6]: PRSCVL, adc clk = PCLK / (PRSCVL + 1)
	 * [5:3] : SEL_MUX, 000 = AIN 0
	 * [2]   : STDBM
	 * [0]   : 1 = A/D conversion starts and this bit is cleared after the startup.
	 */
	ADCCON = (1<<14) | (49<<6) | (channel<<3);

	ADCDLY = 0xff;	
}

int ReadADC(int channel)
{
	InitADC(channel);
	
	/* Æô¶¯ADC */
	ADCCON |= (1<<0);

	while (!(ADCCON & (1<<15)));  /* µÈ´ýADC½áÊø */

	return ADCDAT0 & 0x3ff;
}

