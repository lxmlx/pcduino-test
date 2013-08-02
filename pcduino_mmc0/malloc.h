#ifndef _MALLOC_H
#define _MALLOC_H

#include "types.h"

void* memset(void*, int, size_t);
void * memcpy(void *dest, const void *src, size_t count);
int memcmp(const void * cs,const void * ct,size_t count);

#endif
