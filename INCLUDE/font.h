#ifndef _FONT_H
#define _FONT_H

void InitFont(void);
void PrintFbChar8x16(int x, int y, char c, unsigned int color);
void PrintFbString8x16(int x, int y, char* str, unsigned int color,int isDefer);
void PrintFbString16x32(int x, int y, char* str, unsigned int color,int isDefer);


#endif /* _FONT_H */

