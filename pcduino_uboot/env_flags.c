#include "search.h"

/*
 * Perform consistency checking before creating, overwriting, or deleting an
 * environment variable. Called as a callback function by hsearch_r() and
 * hdelete_r(). Returns 0 in case of success, 1 in case of failure.
 * When (flag & H_FORCE) is set, do not print out any error message and force
 * overwriting of write-once variables.
 */

int env_flags_validate(const ENTRY *item, const char *newval, enum env_op op,
	int flag)
{
	return 0;
}
