#include "types.h"

size_t strlen(const char * s);
size_t strnlen(const char * s, size_t count);
unsigned long long simple_strtoull(const char *cp, char **endp,
					unsigned int base);
char *simple_itoa(ulong i);
