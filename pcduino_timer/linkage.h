#ifndef _LINKAGE_H
#define _LINKAGE_H

#define SYMBOL_NAME_LABEL(X)	X:
#define SYMBOL_NAME(X)		X

#define ALIGN .align		4

#define LENTRY(name) \
	ALIGN; \
	SYMBOL_NAME_LABEL(name)

#define ENTRY(name) \
	.globl SYMBOL_NAME(name); \
	LENTRY(name)

#define END(name) \
	.size name, .-name

#define ENDPROC(name) \
	.type name STT_FUNC; \
	END(name)

#endif
