
#ifndef _FRAMEBUFFER_H
#define _FRAMEBUFFER_H

void GetLcdParamsforFb(void);
void PutFbPixel(int x, int y, unsigned int color);
void ClearScreen(unsigned int color);
unsigned short Convert32BppTo16Bpp(unsigned int rgb);




#endif /* _FRAMEBUFFER_H */

