#include "irda_rawdata.h"
#include "timer.h"

#define NEXT_PLACE(i) ((i+1)&0x3FF)

static SIrdaRawdataEvent g_sEvents[1024];
static int g_iR = 0;
static int g_iW = 0;

static int IsIrEventBufEmpty(void)
{
	return g_iR == g_iW;
}

static int IsIrEventBufFull(void)
{
	return NEXT_PLACE(g_iW) == g_iR;
}

int PutIrEvent(PSIrdaRawdataEvent pd)
{
	if (IsIrEventBufFull())
		return -1;
	g_sEvents[g_iW] = *pd;
	g_iW = NEXT_PLACE(g_iW);
	return 0;
}

int GetIrEvent(PSIrdaRawdataEvent pd)
{
	if (IsIrEventBufEmpty())
		return -1;
	*pd = g_sEvents[g_iR];
	g_iR = NEXT_PLACE(g_iR);
	return 0;
}


int GetIrEventTimeout(PSIrdaRawdataEvent pd, int timeout_us)
{
	while (timeout_us--)
	{
		if (!IsIrEventBufEmpty())
		{
			*pd = g_sEvents[g_iR];
			g_iR = NEXT_PLACE(g_iR);
			
			return 0;
		}
		uDelay(1);
	}
	return -1;
}


