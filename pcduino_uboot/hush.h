#ifndef _HUSH_H
#define _HUSH_H

#define FLAG_EXIT_FROM_LOOP 1
#define FLAG_PARSE_SEMICOLON (1 << 1)	  /* symbol ';' is special for parser */
#define FLAG_REPARSING       (1 << 2)	  /* >=2nd pass */

int u_boot_hush_start(void);

int parse_string_outer(const char *s, int flag);

int set_local_var(const char *s, int flg_export);
void unset_local_var(const char *name);
char *get_local_var(const char *s);

#endif
