#include "types.h"
#include <stdarg.h>

size_t strlen(const char * s);
size_t strnlen(const char * s, size_t count);
unsigned long long simple_strtoull(const char *cp, char **endp,
					unsigned int base);
char *simple_itoa(ulong i);

int sprintf(char *buf, const char *fmt, ...);
int vsprintf(char *buf, const char *fmt, va_list args);
#define snprintf(buf, size, fmt, args...) sprintf(buf, fmt, ##args)
#define scnprintf(buf, size, fmt, args...) sprintf(buf, fmt, ##args)
#define vsnprintf(buf, size, fmt, args...) vsprintf(buf, fmt, ##args)
#define vscnprintf(buf, size, fmt, args...) vsprintf(buf, fmt, ##args)
