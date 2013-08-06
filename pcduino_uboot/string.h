#include "types.h"
#include <stdarg.h>

size_t strlen(const char * s);
size_t strnlen(const char * s, size_t count);
unsigned long long simple_strtoull(const char *cp, char **endp,
					unsigned int base);
char *simple_itoa(ulong i);
int strcmp(const char * cs,const char * ct);
int strncmp(const char * cs,const char * ct,size_t count);
char * strdup(const char *s);
char * strchr(const char * s, int c);
char * strcpy(char * dest,const char *src);
char * strncpy(char * dest,const char *src,size_t count);
char * strstr(const char * s1,const char * s2);
char * strpbrk(const char * cs,const char * ct);
char * strcat(char * dest, const char * src);
char * strncat(char *dest, const char *src, size_t count);

int sprintf(char *buf, const char *fmt, ...);
int vsprintf(char *buf, const char *fmt, va_list args);
#define snprintf(buf, size, fmt, args...) sprintf(buf, fmt, ##args)
#define scnprintf(buf, size, fmt, args...) sprintf(buf, fmt, ##args)
#define vsnprintf(buf, size, fmt, args...) vsprintf(buf, fmt, ##args)
#define vscnprintf(buf, size, fmt, args...) vsprintf(buf, fmt, ##args)
