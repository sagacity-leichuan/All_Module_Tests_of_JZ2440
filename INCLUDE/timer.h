#ifndef _TIMER_H
#define _TIMER_H

#define NULL  ((void *)0)

typedef void(*timer_func)(void);

typedef struct timer_desc {
	char *name;
	timer_func fp;
}timer_desc, *p_timer_desc;

void timer_init(void);
int register_timer(char *name, timer_func fp);
int unregister_timer(char *name);

#endif

