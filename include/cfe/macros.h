#ifndef LIBCFE_CFE_MACROS_H
#define LIBCFE_CFE_MACROS_H


#define MIN(a, b)							\
({									\
	__typeof__(a) _a = (a);						\
	__typeof__(b) _b = (b);						\
	_a <= _b ? _a : _b;						\
})

#define MAX(a, b)							\
({									\
	__typeof__(a) _a = (a);						\
	__typeof__(b) _b = (b);						\
	_a >= _b ? _a : _b;						\
})


#endif
