#include "compiler.h"
#include "interrupts.h"
#include "cache.h"

void __weak cpu_cache_initialization(void){}

int cleanup_before_linux(void)
{
	/*
	 * this function is called just before we call linux
	 * it prepares the processor for linux
	 *
	 * we turn off caches etc ...
	 */
	disable_interrupts();

	/*
	 * Turn off I-cache and invalidate it
	 */
	icache_disable();
	invalidate_icache_all();

	/*
	 * turn off D-cache
	 * dcache_disable() in turn flushes the d-cache and disables MMU
	 */
	dcache_disable();
	v7_outer_cache_disable();

	/*
	 * After D-cache is flushed and before it is disabled there may
	 * be some new valid entries brought into the cache. We are sure
	 * that these lines are not dirty and will not affect our execution.
	 * (because unwinding the call-stack and setting a bit in CP15 SCTRL
	 * is all we did during this. We have not pushed anything on to the
	 * stack. Neither have we affected any static data)
	 * So just invalidate the entire d-cache again to avoid coherency
	 * problems for kernel
	 */
	invalidate_dcache_all();

	/*
	 * Some CPU need more cache attention before starting the kernel.
	 */
	cpu_cache_initialization();

	return 0;
}
