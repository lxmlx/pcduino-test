#ifndef _SUNXI_COMMON_CONFIG_H
#define _SUNXI_COMMON_CONFIG_H

int raise(int signum);
#define CONFIG_CONS_INDEX              1       /* UART0 */

/* A10-EVB has 1 banks of DRAM, we use only one in U-Boot */
#define CONFIG_NR_DRAM_BANKS		1

#define CONFIG_SYS_HZ			1000

/**
 * container_of - cast a member of a structure out to the containing structure
 * @ptr:	the pointer to the member.
 * @type:	the type of the container struct this is embedded in.
 * @member:	the name of the member within the struct.
 *
 */
#define container_of(ptr, type, member) ({			\
	const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
	(type *)( (char *)__mptr - offsetof(type,member) );})


#endif /* _SUNXI_COMMON_CONFIG_H */

