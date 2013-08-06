#ifndef _HUSHTABLE_H
#define _HUSHTABLE_H

#include "search.h"
#include "types.h"

int hsearch_r(ENTRY item, ACTION action, ENTRY ** retval,
	      struct hsearch_data *htab, int flag);

#endif
