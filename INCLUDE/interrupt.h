#ifndef _INTERRUPT_H
#define _INTERRUPT_H

typedef void(*IrqFunc)(int);

/* 初始化中断控制器 */
void InitKeyInterrupt(void);

/* 初始化按键, 设为中断源 */
void InitKeyEint(void);

/* 中断处理函数注册 */
void RegisterIrq(int irq, IrqFunc fp);

#endif

