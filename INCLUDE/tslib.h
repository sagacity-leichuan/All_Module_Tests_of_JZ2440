#ifndef _TSLIB_H
#define _TSLIB_H
#include "function.h"

int CalibrateTs(PSTsParameters tsparam);
int SetTsCalibrateParams(PSTsParameters tsparam);
int ReadTs(int *lcd_x, int *lcd_y, int *lcd_pressure);



#endif

