#ifndef _TIMER_H
#define _TIMER_H

#define NULL  ((void *)0)

typedef void(*TimerFunc)(void);

typedef struct STimerDesc {
	char *name;
	TimerFunc fp;
}STimerDesc, *PSTimerDesc;

void InitTimer(void);
int RegisterTimer(char *name, TimerFunc fp);
int UnregisterTimer(char *name);
int StopTimer(void);
int StartTimer(void);



#endif

