/****************************************************************************************************
  * @brief      : 	JZ2440v2开发板字符串处理功能代码头文件
  * @version    : 	V0.0
  * @note       : 	无
  * @history    : 	无
*****************************************************************************************************/

#ifndef _STRING_UTILS_H
#define _STRING_UTILS_H

#include "uart.h"

long simple_strtol(const char *cp,char **endp,unsigned int base);
char *gets(char s[]);
int get_int(void);
unsigned int get_uint(void);
unsigned int strlen(const char * s);
int strcmp(const char * cs,const char * ct);
void *memcpy(void *dest, const void *src, int count);




#endif /* _STRING_UTILS_H */

