#ifndef __ENV_CALLBACK_H__
#define __ENV_CALLBACK_H__

#include "env_flags.h"
#include "search.h"
#include "linker_lists.h"

#define ENV_CALLBACK_VAR ".callbacks"

/* Board configs can define additional static callback bindings */
#ifndef CONFIG_ENV_CALLBACK_LIST_STATIC
#define CONFIG_ENV_CALLBACK_LIST_STATIC
#endif

#define SILENT_CALLBACK

#define SPLASHIMAGE_CALLBACK

/*
 * This list of callback bindings is static, but may be overridden by defining
 * a new association in the ".callbacks" environment variable.
 */
#define ENV_CALLBACK_LIST_STATIC ENV_CALLBACK_VAR ":callbacks," \
	ENV_FLAGS_VAR ":flags," \
	"baudrate:baudrate," \
	"bootfile:bootfile," \
	"loadaddr:loadaddr," \
	SILENT_CALLBACK \
	SPLASHIMAGE_CALLBACK \
	"stdin:console,stdout:console,stderr:console," \
	CONFIG_ENV_CALLBACK_LIST_STATIC

struct env_clbk_tbl {
	const char *name;		/* Callback name */
	int (*callback)(const char *name, const char *value, enum env_op op,
		int flags);
};

void env_callback_init(ENTRY *var_entry);

/*
 * Define a callback that can be associated with variables.
 * when associated through the ".callbacks" environment variable, the callback
 * will be executed any time the variable is inserted, overwritten, or deleted.
 */
#define U_BOOT_ENV_CALLBACK(name, callback) \
	ll_entry_declare(struct env_clbk_tbl, name, env_clbk) = \
	{#name, callback}

#endif /* __ENV_CALLBACK_H__ */
