#include "search.h"
#include "env_flags.h"
#include "stddef.h"
#include "string.h"
#include "ctype.h"
#include "env_attr.h"
#include "env_common.h"
#include "cmd_nvedit.h"

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

#ifdef CONFIG_CMD_NET
#define ENV_FLAGS_NET_VARTYPE_REPS "im"
#else
#define ENV_FLAGS_NET_VARTYPE_REPS ""
#endif

static const char env_flags_vartype_rep[] = "sdxb" ENV_FLAGS_NET_VARTYPE_REPS;
static const char env_flags_varaccess_rep[] = "aroc";
static const int env_flags_varaccess_mask[] = {
	0,
	ENV_FLAGS_VARACCESS_PREVENT_DELETE |
		ENV_FLAGS_VARACCESS_PREVENT_CREATE |
		ENV_FLAGS_VARACCESS_PREVENT_OVERWR,
	ENV_FLAGS_VARACCESS_PREVENT_DELETE |
		ENV_FLAGS_VARACCESS_PREVENT_OVERWR,
	ENV_FLAGS_VARACCESS_PREVENT_DELETE |
		ENV_FLAGS_VARACCESS_PREVENT_NONDEF_OVERWR};

/*
 * Parse the flags string from a .flags attribute list into the vartype enum.
 */
enum env_flags_vartype env_flags_parse_vartype(const char *flags)
{
	char *type;

	if (strlen(flags) <= ENV_FLAGS_VARTYPE_LOC)
		return env_flags_vartype_string;

	type = strchr(env_flags_vartype_rep,
		flags[ENV_FLAGS_VARTYPE_LOC]);

	if (type != NULL)
		return (enum env_flags_vartype)
			(type - &env_flags_vartype_rep[0]);

	printf("## Warning: Unknown environment variable type '%c'\n",
		flags[ENV_FLAGS_VARTYPE_LOC]);
	return env_flags_vartype_string;
}

/*
 * Parse the flags string from a .flags attribute list into the varaccess enum.
 */
enum env_flags_varaccess env_flags_parse_varaccess(const char *flags)
{
	char *access;

	if (strlen(flags) <= ENV_FLAGS_VARACCESS_LOC)
		return env_flags_varaccess_any;

	access = strchr(env_flags_varaccess_rep,
		flags[ENV_FLAGS_VARACCESS_LOC]);

	if (access != NULL)
		return (enum env_flags_varaccess)
			(access - &env_flags_varaccess_rep[0]);

	printf("## Warning: Unknown environment variable access method '%c'\n",
		flags[ENV_FLAGS_VARACCESS_LOC]);
	return env_flags_varaccess_any;
}

/*
 * Parse the binary flags from a hash table entry into the varaccess enum.
 */
enum env_flags_varaccess env_flags_parse_varaccess_from_binflags(int binflags)
{
	int i;

	for (i = 0; i < sizeof(env_flags_varaccess_mask); i++)
		if (env_flags_varaccess_mask[i] ==
		    (binflags & ENV_FLAGS_VARACCESS_BIN_MASK))
			return (enum env_flags_varaccess)i;

	printf("Warning: Non-standard access flags. (0x%x)\n",
		binflags & ENV_FLAGS_VARACCESS_BIN_MASK);

	return env_flags_varaccess_any;
}

static inline int is_hex_prefix(const char *value)
{
	return value[0] == '0' && (value[1] == 'x' || value[1] == 'X');
}

static void skip_num(int hex, const char *value, const char **end,
	int max_digits)
{
	int i;

	if (hex && is_hex_prefix(value))
		value += 2;

	for (i = max_digits; i != 0; i--) {
		if (hex && !isxdigit(*value))
			break;
		if (!hex && !isdigit(*value))
			break;
		value++;
	}
	if (end != NULL)
		*end = value;
}

/*
 * Based on the declared type enum, validate that the value string complies
 * with that format
 */
static int _env_flags_validate_type(const char *value,
	enum env_flags_vartype type)
{
	const char *end;
#ifdef CONFIG_CMD_NET
	const char *cur;
	int i;
#endif

	switch (type) {
	case env_flags_vartype_string:
		break;
	case env_flags_vartype_decimal:
		skip_num(0, value, &end, -1);
		if (*end != '\0')
			return -1;
		break;
	case env_flags_vartype_hex:
		skip_num(1, value, &end, -1);
		if (*end != '\0')
			return -1;
		if (value + 2 == end && is_hex_prefix(value))
			return -1;
		break;
	case env_flags_vartype_bool:
		if (value[0] != '1' && value[0] != 'y' && value[0] != 't' &&
		    value[0] != 'Y' && value[0] != 'T' &&
		    value[0] != '0' && value[0] != 'n' && value[0] != 'f' &&
		    value[0] != 'N' && value[0] != 'F')
			return -1;
		if (value[1] != '\0')
			return -1;
		break;
#ifdef CONFIG_CMD_NET
	case env_flags_vartype_ipaddr:
		cur = value;
		for (i = 0; i < 4; i++) {
			skip_num(0, cur, &end, 3);
			if (cur == end)
				return -1;
			if (i != 3 && *end != '.')
				return -1;
			if (i == 3 && *end != '\0')
				return -1;
			cur = end + 1;
		}
		break;
	case env_flags_vartype_macaddr:
		cur = value;
		for (i = 0; i < 6; i++) {
			skip_num(1, cur, &end, 2);
			if (cur == end)
				return -1;
			if (cur + 2 == end && is_hex_prefix(cur))
				return -1;
			if (i != 5 && *end != ':')
				return -1;
			if (i == 5 && *end != '\0')
				return -1;
			cur = end + 1;
		}
		break;
#endif
	case env_flags_vartype_end:
		return -1;
	}

	/* OK */
	return 0;
}

/*
 * Look for flags in a provided list and failing that the static list
 */
static inline int env_flags_lookup(const char *flags_list, const char *name,
	char *flags)
{
	int ret = 1;

	if (!flags)
		/* bad parameter */
		return -1;

	/* try the env first */
	if (flags_list)
		ret = env_attr_lookup(flags_list, name, flags);

	if (ret != 0)
		/* if not found in the env, look in the static list */
		ret = env_attr_lookup(ENV_FLAGS_LIST_STATIC, name, flags);

	return ret;
}



/*
 * Parse the flag charachters from the .flags attribute list into the binary
 * form to be stored in the environment entry->flags field.
 */
static int env_parse_flags_to_bin(const char *flags)
{
	int binflags;

	binflags = env_flags_parse_vartype(flags) & ENV_FLAGS_VARTYPE_BIN_MASK;
	binflags |= env_flags_varaccess_mask[env_flags_parse_varaccess(flags)];

	return binflags;
}

/*
 * Look for possible flags for a newly added variable
 * This is called specifically when the variable did not exist in the hash
 * previously, so the blanket update did not find this variable.
 */
void env_flags_init(ENTRY *var_entry)
{
	const char *var_name = var_entry->key;
	const char *flags_list = getenv(ENV_FLAGS_VAR);
	char flags[ENV_FLAGS_ATTR_MAX_LEN + 1] = "";
	int ret = 1;

	/* look in the ".flags" and static for a reference to this variable */
	ret = env_flags_lookup(flags_list, var_name, flags);

	/* if any flags were found, set the binary form to the entry */
	if (!ret && strlen(flags))
		var_entry->flags = env_parse_flags_to_bin(flags);
}

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
	const char *name;
	const char *oldval = NULL;

	if (op != env_op_create)
		oldval = item->data;

	name = item->key;

	/* Default value for NULL to protect string-manipulating functions */
	newval = newval ? : "";

	/* validate the value to match the variable type */
	if (op != env_op_delete) {
		enum env_flags_vartype type = (enum env_flags_vartype)
			(ENV_FLAGS_VARTYPE_BIN_MASK & item->flags);

		if (_env_flags_validate_type(newval, type) < 0) {
			printf("## Error: flags type check failure for "
				"\"%s\" <= \"%s\" (type: %c)\n",
				name, newval, env_flags_vartype_rep[type]);
			return -1;
		}
	}

	/* check for access permission */
	if (flag & H_FORCE)
		return 0;

	switch (op) {
	case env_op_delete:
		if (item->flags & ENV_FLAGS_VARACCESS_PREVENT_DELETE) {
			printf("## Error: Can't delete \"%s\"\n", name);
			return 1;
		}
		break;
	case env_op_overwrite:
		if (item->flags & ENV_FLAGS_VARACCESS_PREVENT_OVERWR) {
			printf("## Error: Can't overwrite \"%s\"\n", name);
			return 1;
		} else if (item->flags &
		    ENV_FLAGS_VARACCESS_PREVENT_NONDEF_OVERWR) {
			const char *defval = getenv_default(name);

			if (defval == NULL)
				defval = "";
			printf("oldval: %s  defval: %s\n", oldval, defval);
			if (strcmp(oldval, defval) != 0) {
				printf("## Error: Can't overwrite \"%s\"\n",
					name);
				return 1;
			}
		}
		break;
	case env_op_create:
		if (item->flags & ENV_FLAGS_VARACCESS_PREVENT_CREATE) {
			printf("## Error: Can't create \"%s\"\n", name);
			return 1;
		}
		break;
	}

	return 0;
}
