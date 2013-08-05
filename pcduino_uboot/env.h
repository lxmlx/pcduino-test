#ifndef _ENV_H
#define _ENV_H

#include "search.h"

void env_relocate(void);
int env_flags_validate(const ENTRY *item, const char *newval, enum env_op op,
	int flag);
void set_default_env(const char *s);

#endif
