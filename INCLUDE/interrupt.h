#ifndef _INTERRUPT_H
#define _INTERRUPT_H

typedef void(*irq_func)(int);

/* 初始化中断控制器 */
void key_interrupt_init(void);

/* 初始化按键, 设为中断源 */
void key_eint_init(void);

/* 中断处理函数注册 */
void register_irq(int irq, irq_func fp);

#endif

