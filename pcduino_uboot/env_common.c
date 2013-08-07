#include "global_data.h"
#include "env_flags.h"
#include "env_default.h"
#include "search.h"
#include "environment.h"
#include "errno.h"
#include "stddef.h"
#include "cmd_nvedit.h"
#include "env_common.h"

DECLARE_GLOBAL_DATA_PTR;

#define UARTPRINTF 1
#define DEBUG 0

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

struct hsearch_data env_htab = {
	.change_ok = env_flags_validate,
};

static uchar __env_get_char_spec(int index)
{
	return *((uchar *)(gd->env_addr + index));
}
uchar env_get_char_spec(int)
	__attribute__((weak, alias("__env_get_char_spec")));

static uchar env_get_char_init(int index)
{
	/* if crc was bad, use the default environment */
	if (gd->env_valid)
		return env_get_char_spec(index);
	else
		return default_environment[index];
}

uchar env_get_char_memory(int index)
{
	return *env_get_addr(index);
}

/*
 * Look up the variable from the default environment
 */
char *getenv_default(const char *name)
{
	char *ret_val;
	unsigned long really_valid = gd->env_valid;
	unsigned long real_gd_flags = gd->flags;

	/* Pretend that the image is bad. */
	gd->flags &= ~GD_FLG_ENV_READY;
	gd->env_valid = 0;
	ret_val = getenv(name);
	gd->env_valid = really_valid;
	gd->flags = real_gd_flags;
	return ret_val;
}

/* 这个函数看起来没什么用啊 */
void set_default_env(const char *s)
{
	int flags = 0;

	if (sizeof(default_environment) > ENV_SIZE) {
		printf(" *** ERROR - default environment is too large\n\n");
		return;
	}

	if (s) {
		if (*s == '!') {
			printf(" *** Warning - %s, "
				"using default environment\n\n", s + 1);
		} else {
			flags = H_INTERACTIVE;
			printf(s);
		}
	} else {
		printf("Using default environment\n\n");
	}

	if (himport_r(&env_htab, (char *)default_environment,
		sizeof(default_environment), '\0', flags,
		0, NULL) == 0)
		printf("Environment import failed: errno = %d\n", errno);

	gd->flags |= GD_FLG_ENV_READY;
}

uchar env_get_char(int index)
{
	/* if relocated to RAM */
	if (gd->flags & GD_FLG_RELOC)
		return env_get_char_memory(index);
	else
		return env_get_char_init(index);
}

const uchar *env_get_addr(int index)
{
	if (gd->env_valid)
		return (uchar *)(gd->env_addr + index);
	else
		return &default_environment[index];
}

/*
 * Read an environment variable as a boolean
 * Return -1 if variable does not exist (default to true)
 */
int getenv_yesno(const char *var)
{
	char *s = getenv(var);

	if (s == NULL)
		return -1;
	return (*s == '1' || *s == 'y' || *s == 'Y' || *s == 't' || *s == 'T') ?
		1 : 0;
}
