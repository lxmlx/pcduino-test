#ifndef _BOOTM_H
#define _BOOTM_H

#include "lmb.h"
#include "image.h"

void arch_lmb_reserve(struct lmb *lmb);
int do_bootm_linux(int flag, int argc, char * const argv[], bootm_headers_t *images);

#endif
