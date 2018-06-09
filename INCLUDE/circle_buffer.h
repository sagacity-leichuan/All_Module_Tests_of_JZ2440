#ifndef _CIRCLE_BUFFER_H
#define _CIRCLE_BUFFER_H

int PutIrEvent(PSIrdaRawdataEvent pd);
int GetIrEvent(PSIrdaRawdataEvent pd);
int GetIrEventTimeout(PSIrdaRawdataEvent pd, int timeout_us);

#endif
