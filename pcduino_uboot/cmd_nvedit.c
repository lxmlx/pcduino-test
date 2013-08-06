#include "stddef.h"
#include "types.h"
#include "search.h"
#include "global_data.h"
#include "hushtable.h"
#include "env_common.h"
#include "common.h"
#include "vsprintf.h"

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

DECLARE_GLOBAL_DATA_PTR;

/*
 * Match a name / name=value pair
 *
 * s1 is either a simple 'name', or a 'name=value' pair.
 * i2 is the environment index for a 'name2=value2' pair.
 * If the names match, return the index for the value2, else -1.
 */
int envmatch(uchar *s1, int i2)
{
	if (s1 == NULL)
		return -1;

	while (*s1 == env_get_char(i2++))
		if (*s1++ == '=')
			return i2;

	if (*s1 == '\0' && env_get_char(i2-1) == '=')
		return i2;

	return -1;
}

/*
 * Look up variable from environment for restricted C runtime env.
 */
int getenv_f(const char *name, char *buf, unsigned len)
{
	int i, nxt;

	for (i = 0; env_get_char(i) != '\0'; i = nxt + 1) {
		int val, n;

		for (nxt = i; env_get_char(nxt) != '\0'; ++nxt) {
			if (nxt >= CONFIG_ENV_SIZE)
				return -1;
		}

		val = envmatch((uchar *)name, i);
		if (val < 0)
			continue;

		/* found; copy out */
		for (n = 0; n < len; ++n, ++buf) {
			*buf = env_get_char(val++);
			if (*buf == '\0')
				return n;
		}

		if (n)
			*--buf = '\0';

		printf("env_buf [%d bytes] too small for value of \"%s\"\n",
			len, name);

		return n;
	}

	return -1;
}

/*
 * Look up variable from environment,
 * return address of storage for that variable,
 * or NULL if not found
 */
char *getenv(const char *name)
{
	if (gd->flags & GD_FLG_ENV_READY) { /* after import into hashtable */
		ENTRY e, *ep;

		e.key	= name;
		e.data	= NULL;
		hsearch_r(e, FIND, &ep, &env_htab, 0);

		return ep ? ep->data : NULL;
	}

	/* restricted capabilities before import */
	if (getenv_f(name, (char *)(gd->env_buf), sizeof(gd->env_buf)) > 0)
		return (char *)(gd->env_buf);

	return NULL;
}

/**
 * Decode the integer value of an environment variable and return it.
 *
 * @param name		Name of environemnt variable
 * @param base		Number base to use (normally 10, or 16 for hex)
 * @param default_val	Default value to return if the variable is not
 *			found
 * @return the decoded value, or default_val if not found
 */
ulong getenv_ulong(const char *name, int base, ulong default_val)
{
	/*
	 * We can use getenv() here, even before relocation, since the
	 * environment variable value is an integer and thus short.
	 */
	const char *str = getenv(name);

	return str ? simple_strtoul(str, NULL, base) : default_val;
}
