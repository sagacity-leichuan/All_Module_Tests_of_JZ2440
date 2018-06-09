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

