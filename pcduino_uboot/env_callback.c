#include "env_callback.h"
#include "stddef.h"
#include "cmd_nvedit.h"
#include "string.h"
#include "env_attr.h"

/*
 * Look up a callback function pointer by name
 */
static struct env_clbk_tbl *find_env_callback(const char *name)
{
	struct env_clbk_tbl *clbkp;
	int i;
	int num_callbacks = ll_entry_count(struct env_clbk_tbl, env_clbk);

	if (name == NULL)
		return NULL;

	/* look up the callback in the linker-list */
	for (i = 0, clbkp = ll_entry_start(struct env_clbk_tbl, env_clbk);
	     i < num_callbacks;
	     i++, clbkp++) {
		if (strcmp(name, clbkp->name) == 0)
			return clbkp;
	}

	return NULL;
}

/*
 * Look for a possible callback for a newly added variable
 * This is called specifically when the variable did not exist in the hash
 * previously, so the blanket update did not find this variable.
 */
void env_callback_init(ENTRY *var_entry)
{
	const char *var_name = var_entry->key;
	const char *callback_list = getenv(ENV_CALLBACK_VAR);
	char callback_name[256] = "";
	struct env_clbk_tbl *clbkp;
	int ret = 1;

	/* look in the ".callbacks" var for a reference to this variable */
	if (callback_list != NULL)
		ret = env_attr_lookup(callback_list, var_name, callback_name);

	/* only if not found there, look in the static list */
	if (ret)
		ret = env_attr_lookup(ENV_CALLBACK_LIST_STATIC, var_name,
			callback_name);

	/* if an association was found, set the callback pointer */
	if (!ret && strlen(callback_name)) {
		clbkp = find_env_callback(callback_name);
		if (clbkp != NULL)
			var_entry->callback = clbkp->callback;
	}
}
