#include "s3c2440_soc.h"
#include "interrupt.h"

IrqFunc g_fIrqArray[32];

void HandleIrqC(void)
{
	/* 分辨中断源 */
	int bit = INTOFFSET;

	g_fIrqArray[bit](bit);
	
	/* 清中断 : 从源头开始清 */
	SRCPND = (1<<bit);
	INTPND = (1<<bit);	
}

void RegisterIrq(int irq, IrqFunc fp)
{
	g_fIrqArray[irq] = fp;

	INTMSK &= ~(1<<irq);
}


