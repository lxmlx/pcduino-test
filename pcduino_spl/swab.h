#ifndef _LINUX_BYTEORDER_SWAB_H
#define _LINUX_BYTEORDER_SWAB_H

#include "types.h"

/*
 * linux/byteorder/swab.h
 * Byte-swapping, independently from CPU endianness
 *	swabXX[ps]?(foo)
 *
 * Francois-Rene Rideau <fare@tunes.org> 19971205
 *    separated swab functions from cpu_to_XX,
 *    to clean up support for bizarre-endian architectures.
 *
 * See asm-i386/byteorder.h and suches for examples of how to provide
 * architecture-dependent optimized versions
 *
 */

/* casts are necessary for constants, because we never know how for sure
 * how U/UL/ULL map to __u16, __u32, __u64. At least not in a portable way.
 */
#define ___swab16(x) \
	((__u16)( \
		(((__u16)(x) & (__u16)0x00ffU) << 8) | \
		(((__u16)(x) & (__u16)0xff00U) >> 8) ))
#define ___swab32(x) \
	((__u32)( \
		(((__u32)(x) & (__u32)0x000000ffUL) << 24) | \
		(((__u32)(x) & (__u32)0x0000ff00UL) <<  8) | \
		(((__u32)(x) & (__u32)0x00ff0000UL) >>  8) | \
		(((__u32)(x) & (__u32)0xff000000UL) >> 24) ))
#define ___swab64(x) \
	((__u64)( \
		(__u64)(((__u64)(x) & (__u64)0x00000000000000ffULL) << 56) | \
		(__u64)(((__u64)(x) & (__u64)0x000000000000ff00ULL) << 40) | \
		(__u64)(((__u64)(x) & (__u64)0x0000000000ff0000ULL) << 24) | \
		(__u64)(((__u64)(x) & (__u64)0x00000000ff000000ULL) <<  8) | \
		(__u64)(((__u64)(x) & (__u64)0x000000ff00000000ULL) >>  8) | \
		(__u64)(((__u64)(x) & (__u64)0x0000ff0000000000ULL) >> 24) | \
		(__u64)(((__u64)(x) & (__u64)0x00ff000000000000ULL) >> 40) | \
		(__u64)(((__u64)(x) & (__u64)0xff00000000000000ULL) >> 56) ))

/*
 * provide defaults when no architecture-specific optimization is detected
 */
#ifndef __arch__swab16
#  define __arch__swab16(x) ___swab16(x)
#endif
#ifndef __arch__swab32
#  define __arch__swab32(x) ___swab32(x)
#endif
#ifndef __arch__swab64
#  define __arch__swab64(x) ___swab64(x)
#endif

#ifndef __arch__swab16p
#  define __arch__swab16p(x) __swab16(*(x))
#endif
#ifndef __arch__swab32p
#  define __arch__swab32p(x) __swab32(*(x))
#endif
#ifndef __arch__swab64p
#  define __arch__swab64p(x) __swab64(*(x))
#endif

#ifndef __arch__swab16s
#  define __arch__swab16s(x) do { *(x) = __swab16p((x)); } while (0)
#endif
#ifndef __arch__swab32s
#  define __arch__swab32s(x) do { *(x) = __swab32p((x)); } while (0)
#endif
#ifndef __arch__swab64s
#  define __arch__swab64s(x) do { *(x) = __swab64p((x)); } while (0)
#endif

static __inline__ __attribute__((const)) __u32 __fswab32(__u32 x)
{
	return __arch__swab32(x);
}

/*
 * Allow constant folding
 */
#  define __swab16(x) \
(__builtin_constant_p((__u16)(x)) ? \
 ___swab16((x)) : \
 __fswab16((x)))
#  define __swab32(x) \
(__builtin_constant_p((__u32)(x)) ? \
 ___swab32((x)) : \
 __fswab32((x)))
#  define __swab64(x) \
(__builtin_constant_p((__u64)(x)) ? \
 ___swab64((x)) : \
 __fswab64((x)))

#endif
