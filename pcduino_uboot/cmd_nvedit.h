#ifndef _CMD_NVEDIT_H
#define _CMD_NVEDIT_H

#include "types.h"

char *getenv(const char *name);
ulong getenv_ulong(const char *name, int base, ulong default_val);

#endif
