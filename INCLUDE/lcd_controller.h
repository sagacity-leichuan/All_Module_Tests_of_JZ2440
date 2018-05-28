
#ifndef _LCD_CONTROLLER_H
#define _LCD_CONTROLLER_H

#include "lcd.h"

typedef struct SLcdController {
	char *pchName;
	void (*Init)(PSLcdParams plcdparams);
	void (*Enable)(void);
	void (*Disable)(void);
	void (*InitPalette)(void);
}SLcdController, *PSLcdController;


void AddLcdContollerS3c2440(void);
int InitLcdController(PSLcdParams plcdparams);
int RegisterLcdController(PSLcdController plcdcon);
void EnableLcdController(void);
void DisableLcdController(void);
int SelectLcdController(char *name);



#endif /* _LCD_CONTROLLER_H */

