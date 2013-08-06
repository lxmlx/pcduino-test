#include "timer.h"
#include "hush.h"
#include "cmd_nvedit.h"
#include "sunxi-common.h"
#include "vsprintf.h"
#include "stddef.h"
#include "uart.h"
#include "malloc.h"
#include "string.h"

#define DEBUG 1

#if DEBUG
#define debug(fmt, args...) printf(fmt, ##args)
#else
#define debug(fmt, args...)
#endif

int run_command_list(const char *cmd, int len, int flag)
{
	int need_buff = 1;
	char *buff = (char *)cmd;	/* cast away const */
	int rcode = 0;

	if (len == -1) {
		len = strlen(cmd);
		
		/* hush will never change our string */
		need_buff = 0;
	}
	if (need_buff) {
		buff = malloc(len + 1);
		if (!buff)
			return 1;
		memcpy(buff, cmd, len);
		buff[len] = '\0';
	}

	rcode = parse_string_outer(buff, FLAG_PARSE_SEMICOLON);

	return rcode;
}

static int abortboot_normal(int bootdelay)
{
	int abort = 0;
	unsigned long ts;

	if (bootdelay >= 0)
		printf("Hit any key to stop autoboot: %2d ", bootdelay);

	while ((bootdelay > 0) && (!abort)) {
		--bootdelay;
		/* delay 1000 ms */
		ts = get_mtimer(0);
		do {
			if (uart_testchar(0)) {	/* we got a key press	*/
				abort  = 1;	/* don't auto boot	*/
				bootdelay = 0;	/* no more delay	*/
				(void) uart_getchar();  /* consume input	*/
				break;
			}
			mdelay(10);
		} while (!abort && get_mtimer(ts) < 1000);

		printf("\b\b\b%2d ", bootdelay);
	}

	uart_putchar('\n');

	return abort;
}

static int abortboot(int bootdelay)
{
	return abortboot_normal(bootdelay);
}

static void process_boot_delay(void)
{
	char *s;
	int bootdelay;

	s = getenv("bootdelay");
	bootdelay = s ? (int)simple_strtol(s, NULL, 10) : CONFIG_BOOTDELAY;
	debug(" ### main_loop entered: bootdelay=%d\n\n", bootdelay);

	s = getenv("bootcmd");
	debug(" ### main_loop: bootcmd=\"%s\"\n", s ? s : "<UNDEFINED>");

	if (bootdelay != -1 && s && !abortboot(bootdelay))
		run_command_list(s, -1, 0);
}

void main_loop(void)
{
	u_boot_hush_start();
	process_boot_delay();
	/*
	 * Main Loop for Monitor Command Processing
	 */
	/* parse_file_outer(); */
	/* This point is never reached */
	while(1) {
		printf("z");
		mdelay(1000);
	}
}
