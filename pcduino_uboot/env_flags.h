#ifndef _ENV_FLAGS_H
#define _ENV_FLAGS_H

#include "search.h"

enum env_flags_vartype {
	env_flags_vartype_string,
	env_flags_vartype_decimal,
	env_flags_vartype_hex,
	env_flags_vartype_bool,
#ifdef CONFIG_CMD_NET
	env_flags_vartype_ipaddr,
	env_flags_vartype_macaddr,
#endif
	env_flags_vartype_end
};

enum env_flags_varaccess {
	env_flags_varaccess_any,
	env_flags_varaccess_readonly,
	env_flags_varaccess_writeonce,
	env_flags_varaccess_changedefault,
	env_flags_varaccess_end
};

#define ENV_FLAGS_VAR ".flags"
#define ENV_FLAGS_ATTR_MAX_LEN 2
#define ENV_FLAGS_VARTYPE_LOC 0
#define ENV_FLAGS_VARACCESS_LOC 1

#ifndef CONFIG_ENV_FLAGS_LIST_STATIC
#define CONFIG_ENV_FLAGS_LIST_STATIC ""
#endif

#ifdef CONFIG_CMD_NET
#define ETHADDR_FLAGS "ethaddr:mo,"
#else
#define ETHADDR_FLAGS ""
#endif

#define SERIAL_FLAGS "serial#:so,"

#define ENV_FLAGS_LIST_STATIC \
	ETHADDR_FLAGS \
	SERIAL_FLAGS \
	CONFIG_ENV_FLAGS_LIST_STATIC

/*
 * Parse the flags string from a .flags attribute list into the vartype enum.
 */
enum env_flags_vartype env_flags_parse_vartype(const char *flags);
/*
 * Parse the flags string from a .flags attribute list into the varaccess enum.
 */
enum env_flags_varaccess env_flags_parse_varaccess(const char *flags);
/*
 * Parse the binary flags from a hash table entry into the varaccess enum.
 */
enum env_flags_varaccess env_flags_parse_varaccess_from_binflags(int binflags);


/*
 * When adding a variable to the environment, initialize the flags for that
 * variable.
 */
void env_flags_init(ENTRY *var_entry);

/*
 * Validate the newval for to conform with the requirements defined by its flags
 */
int env_flags_validate(const ENTRY *item, const char *newval, enum env_op op,
	int flag);

/*
 * These are the binary flags used in the environment entry->flags variable to
 * decribe properties of veriables in the table
 */
#define ENV_FLAGS_VARTYPE_BIN_MASK			0x00000007
/* The actual variable type values use the enum value (within the mask) */
#define ENV_FLAGS_VARACCESS_PREVENT_DELETE		0x00000008
#define ENV_FLAGS_VARACCESS_PREVENT_CREATE		0x00000010
#define ENV_FLAGS_VARACCESS_PREVENT_OVERWR		0x00000020
#define ENV_FLAGS_VARACCESS_PREVENT_NONDEF_OVERWR	0x00000040
#define ENV_FLAGS_VARACCESS_BIN_MASK			0x00000078

#endif /* __ENV_FLAGS_H__ */
