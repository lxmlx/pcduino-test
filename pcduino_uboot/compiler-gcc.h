#ifndef _COMPILE_H
#error "Please don't include compiler-gcc.h directly, include compiler.h instead."
#endif

#define __deprecated			__attribute__((deprecated))
#define __packed			__attribute__((packed))
#define __weak				__attribute__((weak))
