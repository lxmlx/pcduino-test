#include "global_data.h"
#include "env.h"
#include "env_default.h"
#include "search.h"
#include "environment.h"
#include "errno.h"
#include "stddef.h"

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

void env_relocate(void)
{
	if (gd->env_valid == 0) {
		set_default_env("!Bad CRC");
	} else {
		//env_relocate_spec();
	}
}
