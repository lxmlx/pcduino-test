#ifndef _ENV_COMMON_H
#define _ENV_COMMON_H

#include "common.h"

extern struct hsearch_data env_htab;
char *getenv_default(const char *name);
void set_default_env(const char *s);

uchar env_get_char(int index);
const uchar *env_get_addr(int index);

#endif
