#ifndef _INTERRUPT_H
#define _INTERRUPT_H

typedef void(*IrqFunc)(int);

void RegisterIrq(int irq, IrqFunc fp);


#endif

