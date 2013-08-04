#ifndef _LITTLE_ENDIAN_H
#define _LITTLE_ENDIAN_H

#include "types.h"
#include "swab.h"
#include "compiler.h"

#define __be32_to_cpu(x) __swab32((__force __u32)(__be32)(x))

#endif
