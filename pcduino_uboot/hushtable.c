#include "search.h"
#include "errno.h"
#include "stddef.h"
#include "types.h"
#include "malloc.h"
#include "ctype.h"
#include "string.h"
#include "hushtable.h"
#include "env_flags.h"
#include "env_callback.h"

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

#ifndef	CONFIG_ENV_MIN_ENTRIES	/* minimum number of entries */
#define	CONFIG_ENV_MIN_ENTRIES 64
#endif
#ifndef	CONFIG_ENV_MAX_ENTRIES	/* maximum number of entries */
#define	CONFIG_ENV_MAX_ENTRIES 512
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
 * hdelete()
 */

/*
 * The standard implementation of hsearch(3) does not provide any way
 * to delete any entries from the hash table.  We extend the code to
 * do that.
 */

static void _hdelete(const char *key, struct hsearch_data *htab, ENTRY *ep,
	int idx)
{
	/* free used ENTRY */
	debug("hdelete: DELETING key \"%s\"\n", key);
	free((void *)ep->key);
	free(ep->data);
	ep->callback = NULL;
	ep->flags = 0;
	htab->table[idx].used = -1;

	--htab->filled;
}

int hdelete_r(const char *key, struct hsearch_data *htab, int flag)
{
	ENTRY e, *ep;
	int idx;

	debug("hdelete: DELETE key \"%s\"\n", key);

	e.key = (char *)key;

	idx = hsearch_r(e, FIND, &ep, htab, 0);
	if (idx == 0) {
		__set_errno(ESRCH);
		return 0;	/* not found */
	}

	/* Check for permission */
	if (htab->change_ok != NULL &&
	    htab->change_ok(ep, NULL, env_op_delete, flag)) {
		debug("change_ok() rejected deleting variable "
			"%s, skipping it!\n", key);
		__set_errno(EPERM);
		return 0;
	}

	/* If there is a callback, call it */
	if (htab->table[idx].entry.callback &&
	    htab->table[idx].entry.callback(key, NULL, env_op_delete, flag)) {
		debug("callback() rejected deleting variable "
			"%s, skipping it!\n", key);
		__set_errno(EINVAL);
		return 0;
	}

	_hdelete(key, htab, ep, idx);

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
 * hsearch()
 */

/*
 * This is the search function. It uses double hashing with open addressing.
 * The argument item.key has to be a pointer to an zero terminated, most
 * probably strings of chars. The function for generating a number of the
 * strings is simple but fast. It can be replaced by a more complex function
 * like ajw (see [Aho,Sethi,Ullman]) if the needs are shown.
 *
 * We use an trick to speed up the lookup. The table is created by hcreate
 * with one more element available. This enables us to use the index zero
 * special. This index will never be used because we store the first hash
 * index in the field used where zero means not used. Every other value
 * means used. The used field can be used as a first fast comparison for
 * equality of the stored and the parameter value. This helps to prevent
 * unnecessary expensive calls of strcmp.
 *
 * This implementation differs from the standard library version of
 * this function in a number of ways:
 *
 * - While the standard version does not make any assumptions about
 *   the type of the stored data objects at all, this implementation
 *   works with NUL terminated strings only.
 * - Instead of storing just pointers to the original objects, we
 *   create local copies so the caller does not need to care about the
 *   data any more.
 * - The standard implementation does not provide a way to update an
 *   existing entry.  This version will create a new entry or update an
 *   existing one when both "action == ENTER" and "item.data != NULL".
 * - Instead of returning 1 on success, we return the index into the
 *   internal hash table, which is also guaranteed to be positive.
 *   This allows us direct access to the found hash table slot for
 *   example for functions like hdelete().
 */

int hmatch_r(const char *match, int last_idx, ENTRY ** retval,
	     struct hsearch_data *htab)
{
	unsigned int idx;
	size_t key_len = strlen(match);

	for (idx = last_idx + 1; idx < htab->size; ++idx) {
		if (htab->table[idx].used <= 0)
			continue;
		if (!strncmp(match, htab->table[idx].entry.key, key_len)) {
			*retval = &htab->table[idx].entry;
			return idx;
		}
	}

	__set_errno(ESRCH);
	*retval = NULL;
	return 0;
}

/*
 * Compare an existing entry with the desired key, and overwrite if the action
 * is ENTER.  This is simply a helper function for hsearch_r().
 */
static inline int _compare_and_overwrite_entry(ENTRY item, ACTION action,
	ENTRY **retval, struct hsearch_data *htab, int flag,
	unsigned int hval, unsigned int idx)
{
	if (htab->table[idx].used == hval
	    && strcmp(item.key, htab->table[idx].entry.key) == 0) {
		/* Overwrite existing value? */
		if ((action == ENTER) && (item.data != NULL)) {
			/* check for permission */
			if (htab->change_ok != NULL && htab->change_ok(
			    &htab->table[idx].entry, item.data,
			    env_op_overwrite, flag)) {
				debug("change_ok() rejected setting variable "
					"%s, skipping it!\n", item.key);
				__set_errno(EPERM);
				*retval = NULL;
				return 0;
			}

			/* If there is a callback, call it */
			if (htab->table[idx].entry.callback &&
			    htab->table[idx].entry.callback(item.key,
			    item.data, env_op_overwrite, flag)) {
				debug("callback() rejected setting variable "
					"%s, skipping it!\n", item.key);
				__set_errno(EINVAL);
				*retval = NULL;
				return 0;
			}

			free(htab->table[idx].entry.data);
			htab->table[idx].entry.data = strdup(item.data);
			if (!htab->table[idx].entry.data) {
				__set_errno(ENOMEM);
				*retval = NULL;
				return 0;
			}
		}
		/* return found entry */
		*retval = &htab->table[idx].entry;
		return idx;
	}
	/* keep searching */
	return -1;
}

int hsearch_r(ENTRY item, ACTION action, ENTRY ** retval,
	      struct hsearch_data *htab, int flag)
{
	unsigned int hval;
	unsigned int count;
	unsigned int len = strlen(item.key);
	unsigned int idx;
	unsigned int first_deleted = 0;
	int ret;

	/* Compute an value for the given string. Perhaps use a better method. */
	hval = len;
	count = len;
	while (count-- > 0) {
		hval <<= 4;
		hval += item.key[count];
	}

	/*
	 * First hash function:
	 * simply take the modul but prevent zero.
	 */
	hval %= htab->size;
	if (hval == 0)
		++hval;

	/* The first index tried. */
	idx = hval;

	if (htab->table[idx].used) {
		/*
		 * Further action might be required according to the
		 * action value.
		 */
		unsigned hval2;

		if (htab->table[idx].used == -1
		    && !first_deleted)
			first_deleted = idx;

		ret = _compare_and_overwrite_entry(item, action, retval, htab,
			flag, hval, idx);
		if (ret != -1)
			return ret;

		/*
		 * Second hash function:
		 * as suggested in [Knuth]
		 */
		hval2 = 1 + hval % (htab->size - 2);

		do {
			/*
			 * Because SIZE is prime this guarantees to
			 * step through all available indices.
			 */
			if (idx <= hval2)
				idx = htab->size + idx - hval2;
			else
				idx -= hval2;

			/*
			 * If we visited all entries leave the loop
			 * unsuccessfully.
			 */
			if (idx == hval)
				break;

			/* If entry is found use it. */
			ret = _compare_and_overwrite_entry(item, action, retval,
				htab, flag, hval, idx);
			if (ret != -1)
				return ret;
		}
		while (htab->table[idx].used);
	}

	/* An empty bucket has been found. */
	if (action == ENTER) {
		/*
		 * If table is full and another entry should be
		 * entered return with error.
		 */
		if (htab->filled == htab->size) {
			__set_errno(ENOMEM);
			*retval = NULL;
			return 0;
		}

		/*
		 * Create new entry;
		 * create copies of item.key and item.data
		 */
		if (first_deleted)
			idx = first_deleted;

		htab->table[idx].used = hval;
		htab->table[idx].entry.key = strdup(item.key);
		htab->table[idx].entry.data = strdup(item.data);
		if (!htab->table[idx].entry.key ||
		    !htab->table[idx].entry.data) {
			__set_errno(ENOMEM);
			*retval = NULL;
			return 0;
		}

		++htab->filled;

		/* This is a new entry, so look up a possible callback */
		env_callback_init(&htab->table[idx].entry);
		/* Also look for flags */
		env_flags_init(&htab->table[idx].entry);

		/* check for permission */
		if (htab->change_ok != NULL && htab->change_ok(
		    &htab->table[idx].entry, item.data, env_op_create, flag)) {
			debug("change_ok() rejected setting variable "
				"%s, skipping it!\n", item.key);
			_hdelete(item.key, htab, &htab->table[idx].entry, idx);
			__set_errno(EPERM);
			*retval = NULL;
			return 0;
		}

		/* If there is a callback, call it */
		if (htab->table[idx].entry.callback &&
		    htab->table[idx].entry.callback(item.key, item.data,
		    env_op_create, flag)) {
			debug("callback() rejected setting variable "
				"%s, skipping it!\n", item.key);
			_hdelete(item.key, htab, &htab->table[idx].entry, idx);
			__set_errno(EINVAL);
			*retval = NULL;
			return 0;
		}

		/* return new entry */
		*retval = &htab->table[idx].entry;
		return 1;
	}

	__set_errno(ESRCH);
	*retval = NULL;
	return 0;
}

/*
 * himport()
 */

/*
 * Check whether variable 'name' is amongst vars[],
 * and remove all instances by setting the pointer to NULL
 */
static int drop_var_from_set(const char *name, int nvars, char * vars[])
{
	int i = 0;
	int res = 0;

	/* No variables specified means process all of them */
	if (nvars == 0)
		return 1;

	for (i = 0; i < nvars; i++) {
		if (vars[i] == NULL)
			continue;
		/* If we found it, delete all of them */
		if (!strcmp(name, vars[i])) {
			vars[i] = NULL;
			res = 1;
		}
	}
	if (!res)
		debug("Skipping non-listed variable %s\n", name);

	return res;
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

	/*
	 * Create new hash table (if needed).  The computation of the hash
	 * table size is based on heuristics: in a sample of some 70+
	 * existing systems we found an average size of 39+ bytes per entry
	 * in the environment (for the whole key=value pair). Assuming a
	 * size of 8 per entry (= safety factor of ~5) should provide enough
	 * safety margin for any existing environment definitions and still
	 * allow for more than enough dynamic additions. Note that the
	 * "size" argument is supposed to give the maximum enviroment size
	 * (CONFIG_ENV_SIZE).  This heuristics will result in
	 * unreasonably large numbers (and thus memory footprint) for
	 * big flash environments (>8,000 entries for 64 KB
	 * envrionment size), so we clip it to a reasonable value.
	 * On the other hand we need to add some more entries for free
	 * space when importing very small buffers. Both boundaries can
	 * be overwritten in the board config file if needed.
	 */

	if (!htab->table) {
		int nent = CONFIG_ENV_MIN_ENTRIES + size / 8;

		if (nent > CONFIG_ENV_MAX_ENTRIES)
			nent = CONFIG_ENV_MAX_ENTRIES;

		debug("Create Hash Table: N=%d\n", nent);

		if (hcreate_r(nent, htab) == 0) {
			free(data);
			return 0;
		}
	}

	/* Parse environment; allow for '\0' and 'sep' as separators */
	do {
		ENTRY e, *rv;

		/* skip leading white space */
		while (isblank(*dp))
			++dp;

		/* skip comment lines */
		if (*dp == '#') {
			while (*dp && (*dp != sep))
				++dp;
			++dp;
			continue;
		}

		/* parse name */
		for (name = dp; (*dp != '=') && (*dp) && (*dp != sep); ++dp)
			;

		/* deal with "name" and "name=" entries (delete var) */
		if (*dp == '\0' || *(dp + 1) == '\0' ||
			*dp == sep || *(dp + 1) == sep) {
			if (*dp == '=')
				*dp++ = '\0';
			*dp++ = '\0';

			debug("DELETE CANDIDATE: \"%s\"\n", name);
			if (!drop_var_from_set(name, nvars, localvars))
				continue;

			if (hdelete_r(name, htab, flag) == 0)
				debug("DELETE ERROR ####################\n");

			continue;
		}
		*dp++ = '\0'; /* terminate name */

		/* parse value; deal with escapes */
		for (value = sp = dp; (*dp) && (*dp != sep); ++dp ) {
			if ((*dp == '\\') && *(dp +1))
				++dp;
			*sp++ = *dp;
		}
		*sp++ = '\0'; /* terminate value */
		++dp;

		if (*name == 0) {
			debug("INSERT: unable to use an empty key\n");
			__set_errno(EINVAL);
			return 0;
		}

		/* Skip variables which are not supposed to be processed */
		if (!drop_var_from_set(name, nvars, localvars))
			continue;

		/* enter into hash table */
		e.key = name;
		e.data = value;

		hsearch_r(e, ENTER, &rv, htab, flag);
		if (rv == NULL)
			printf("himport_r: can't insert \"%s=%s\" "
				"into hash table\n", name, value);
		debug("INSERT: table %p, filled %d/%d rv %p ==> "
			"name=\"%s\" value=\"%s\"\n", htab, htab->filled,
			htab->size, rv, name, value);
	} while((dp < data + size) && *dp);

	debug("INSERT: free(data = %p)\n", data);
	free(data);

	/* process variables which were not considered */
	for (i = 0; i < nvars; i++) {
		if (localvars[i] == NULL)
			continue;
		/*
		 * All variables which were not deleted from the variable list
		 * were not present in the imported env
		 * This could mean two things:
		 * a) if the variable was present in current env, we delete it
		 * b) if the variable was not present in current env, we notify
		 *    it might be a typo
		 */

		if (hdelete_r(localvars[i], htab, flag) == 0)
			printf("WARNING: '%s' neither in running nor "
				"in imported env!\n", localvars[i]);
		else
			printf("WARNING: '%s' not in imported env, "
				"deleting it!\n", localvars[i]);

	}

	debug("INSERT: done\n");
	return 1;		/* everything OK */
}
