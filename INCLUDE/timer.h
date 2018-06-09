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
void uDelay(int n);
void mDelay(int m);
unsigned long long GetSystemTimeUs(void);
unsigned int DeltaTimeUs(unsigned long long pre, unsigned long long now);

#endif

