#ifndef _ASM_CACHE_H
#define _ASM_CACHE_H

#define ARCH_DMA_MINALIGN	64

int	dcache_status (void);
void	dcache_enable (void);
void	dcache_disable(void);
void flush_dcache_all(void);
void  flush_cache(unsigned long start, unsigned long size);

void icache_disable(void);

void invalidate_icache_all(void);
void v7_outer_cache_disable(void);
void invalidate_dcache_all(void);

#endif
