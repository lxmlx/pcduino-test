#ifndef _ASM_CACHE_H
#define _ASM_CACHE_H

#define ARCH_DMA_MINALIGN	64

int	dcache_status (void);
void	dcache_enable (void);
void	dcache_disable(void);
void flush_dcache_all(void);

#endif
