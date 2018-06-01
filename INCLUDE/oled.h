#ifndef  _OLED_H
#define  _OLED_H

void InitOLED(void);
void PrintOLED(int page, int col, char *str);
void PutOLEDChar(int page, int col, char c);

#endif

