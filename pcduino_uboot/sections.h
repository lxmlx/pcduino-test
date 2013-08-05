#ifndef _ASM_GENERIC_SECTIONS_H_
#define _ASM_GENERIC_SECTIONS_H_

extern ulong _bss_start_ofs;	/* BSS start relative to _start */
extern ulong _bss_end_ofs;		/* BSS end relative to _start */
extern ulong _end_ofs;		/* end of image relative to _start */
extern ulong _TEXT_BASE;	/* code start */

#endif
