#ifndef _TOUCHSCREEN_H
#define _TOUCHSCREEN_H

void InitTouchScreen(void);
void ReadTsRaw(int *px, int *py, int *ppressure);
void GetLcdParams(unsigned int *fb_base, int *xres, int *yres, int *bpp);
void ReportTsXY(int x, int y, int pressure);
void ReadTsRawTest(int *px, int *py, int *ppressure);

void TestTouchScreen(void);



#endif /* _TOUCHSCREEN_H */

