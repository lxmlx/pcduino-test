#include "search.h"
#include "errno.h"
#include "stddef.h"
#include "types.h"
#include "malloc.h"

#define UARTPRINTF 1
#define DEBUG 1

#if UARTPRINTF
#include "uart.h"
	#if DEBUG
	#define debug(fmt, args...) printf(fmt, ##args)
	#else
	#define debug(fmt, args...)
	#endif
#else
#define debug(fmt, args...)
#define printf(fmt, args...)
#endif

/*
 * The reentrant version has no static variables to maintain the state.
 * Instead the interface of all functions is extended to take an argument
 * which describes the current status.
 */

typedef struct _ENTRY {
	int used;
	ENTRY entry;
} _ENTRY;


/*
 * hcreate()
 */

/*
 * For the used double hash method the table size has to be a prime. To
 * correct the user given table size we need a prime test.  This trivial
 * algorithm is adequate because
 * a)  the code is (most probably) called a few times per program run and
 * b)  the number is small because the table must fit in the core
 * */
static int isprime(unsigned int number)
{
	/* no even number will be passed */
	unsigned int div = 3;

	while (div * div < number && number % div != 0)
		div += 2;

	return number % div != 0;
}

/*
 * Before using the hash table we must allocate memory for it.
 * Test for an existing table are done. We allocate one element
 * more as the found prime number says. This is done for more effective
 * indexing as explained in the comment for the hsearch function.
 * The contents of the table is zeroed, especially the field used
 * becomes zero.
 */

int hcreate_r(size_t nel, struct hsearch_data *htab)
{
	/* Test for correct arguments.  */
	if (htab == NULL) {
		__set_errno(EINVAL);
		return 0;
	}

	/* There is still another table active. Return with error. */
	if (htab->table != NULL)
		return 0;

	/* Change nel to the first prime number not smaller as nel. */
	nel |= 1;		/* make odd */
	while (!isprime(nel))
		nel += 2;

	htab->size = nel;
	htab->filled = 0;

	/* allocate memory and zero out */
	htab->table = (_ENTRY *) calloc(htab->size + 1, sizeof(_ENTRY));
	if (htab->table == NULL)
		return 0;

	/* everything went alright */
	return 1;
}

/*
 * hdestroy()
 */

/*
 * After using the hash table it has to be destroyed. The used memory can
 * be freed and the local static variable can be marked as not used.
 */

void hdestroy_r(struct hsearch_data *htab)
{
	int i;

	/* Test for correct arguments.  */
	if (htab == NULL) {
		__set_errno(EINVAL);
		return;
	}

	/* free used memory */
	for (i = 1; i <= htab->size; ++i) {
		if (htab->table[i].used > 0) {
			ENTRY *ep = &htab->table[i].entry;

			free((void *)ep->key);
			free(ep->data);
		}
	}
	free(htab->table);

	/* the sign for an existing table is an value != NULL in htable */
	htab->table = NULL;
}

/*
 * Import linearized data into hash table.
 *
 * This is the inverse function to hexport(): it takes a linear list
 * of "name=value" pairs and creates hash table entries from it.
 *
 * Entries without "value", i. e. consisting of only "name" or
 * "name=", will cause this entry to be deleted from the hash table.
 *
 * The "flag" argument can be used to control the behaviour: when the
 * H_NOCLEAR bit is set, then an existing hash table will kept, i. e.
 * new data will be added to an existing hash table; otherwise, old
 * data will be discarded and a new hash table will be created.
 *
 * The separator character for the "name=value" pairs can be selected,
 * so we both support importing from externally stored environment
 * data (separated by NUL characters) and from plain text files
 * (entries separated by newline characters).
 *
 * To allow for nicely formatted text input, leading white space
 * (sequences of SPACE and TAB chars) is ignored, and entries starting
 * (after removal of any leading white space) with a '#' character are
 * considered comments and ignored.
 *
 * [NOTE: this means that a variable name cannot start with a '#'
 * character.]
 *
 * When using a non-NUL separator character, backslash is used as
 * escape character in the value part, allowing for example for
 * multi-line values.
 *
 * In theory, arbitrary separator characters can be used, but only
 * '\0' and '\n' have really been tested.
 */

int himport_r(struct hsearch_data *htab,
	const char *env, size_t size, const char sep, int flag,
	int nvars, char *const vars[])
{
	char *data, *sp, *dp, *name, *value;
	char *localvars[nvars];
	int i;

	/* Test for correct arguments. */
	if (htab == NULL) {
		__set_errno(EINVAL);
		return 0;
	}

	/* We allocate new space to make sure we can write to the array */
	if ((data = malloc(size)) == NULL) {
		debug("himport_r: can't malloc %zu bytes\n", size);
		__set_errno(ENOMEM);
		return 0;
	}
	memcpy(data, env, size);
	dp = data;

	/* make a local copy of the list of variables */
	if (nvars)
		memcpy(localvars, vars, sizeof(vars[0]) * nvars);

	if ((flag & H_NOCLEAR) == 0) {
		/* Destroy old hash table if one exists */
		debug("Destroy Hash Table: %p table = %p\n", htab,
		       htab->table);
		if (htab->table)
			hdestroy_r(htab);
	}

	return 0;
}
