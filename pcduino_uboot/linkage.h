#ifndef _LINUX_LINKAGE_H
#define _LINUX_LINKAGE_H

#define CPP_ASMLINKAGE

#define asmlinkage CPP_ASMLINKAGE

#define SYMBOL_NAME_STR(X)	#X
#define SYMBOL_NAME(X)		X
#define SYMBOL_NAME_LABEL(X)	X##:

#define __ALIGN .align 0
#define __ALIGN_STR ".align 0"

#define ALIGN			__ALIGN
#define ALIGN_STR		__ALIGN_STR

#define LENTRY(name) \
	ALIGN; \
	SYMBOL_NAME_LABEL(name)

#define ENTRY(name) \
	.globl SYMBOL_NAME(name); \
	LENTRY(name)

#ifndef END
#define END(name) \
	.size name, .-name
#endif

#ifndef ENDPROC
#define ENDPROC(name) \
	.type name STT_FUNC; \
	END(name)
#endif

#endif
