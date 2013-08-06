#include "command.h"
#include "uart.h"

#define DEBUG 1

#if DEBUG
#define debug(fmt, args...) printf(fmt, ##args)
#else
#define debug(fmt, args...)
#endif

enum command_ret_t cmd_process(int flag, int argc, char * const argv[],
			       int *repeatable, ulong *ticks)
{
	enum command_ret_t rc = CMD_RET_SUCCESS;
	int i;

	debug("cmd_process: flag: %d, argc: %d, argv: ",
		flag, argc);
	for(i = 0; i < argc; i++)
		debug("%s, ", argv[i]);
	debug("\n");

	return rc;
}
