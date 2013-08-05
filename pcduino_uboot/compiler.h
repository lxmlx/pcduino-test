#ifndef _COMPILE_H
#define _COMPILE_H

#include "compiler-gcc.h"

# define cpu_to_le16(x)		(x)
# define cpu_to_le32(x)		(x)
# define cpu_to_le64(x)		(x)
# define le16_to_cpu(x)		(x)
# define le32_to_cpu(x)		(x)
# define le64_to_cpu(x)		(x)

# define __force

/* Type for `void *' pointers. */
typedef unsigned long int uintptr_t;

#endif
