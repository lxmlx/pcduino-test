#ifndef _UBOOT_CRC_H
#define _UBOOT_CRC_H

#include "types.h"

/* lib/crc32.c */
uint32_t crc32 (uint32_t, const unsigned char *, uint);
uint32_t crc32_wd (uint32_t, const unsigned char *, uint, uint);
uint32_t crc32_no_comp (uint32_t, const unsigned char *, uint);

#endif /* _UBOOT_CRC_H */
