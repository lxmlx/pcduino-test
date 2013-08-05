#ifndef _ENV_DEFAULT_H
#define _ENV_DEFAULT_H

#include "stringify.h"
#include "common.h"
#include "sunxi-common.h"

const uchar default_environment[] = {
	"bootcmd=" CONFIG_BOOTCOMMAND "\0"
	"bootdelay=" __stringify(CONFIG_BOOTDELAY) "\0"
	"\0"
};

#endif
