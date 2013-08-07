#ifndef _VSPRINTF_H
#define _VSPRINTF_H

unsigned long simple_strtoul(const char *cp, char **endp,
				unsigned int base);
long simple_strtol(const char *cp, char **endp, unsigned int base);
unsigned long long simple_strtoull(const char *cp, char **endp,
					unsigned int base);

#endif
