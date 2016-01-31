#ifndef LIBCFE_COMPILER_H
#error "Do not include compiler-gcc.h directly. Include compiler.h instead"
#endif


#define GCC_VERSION (__GNUC__ * 10000		\
		     + __GNUC_MINOR__ * 100	\
		     + __GNUC_PATCHLEVEL__)


#ifdef DEBUG
#define _must_be_array_(a)		BUILD_BUG_ON_ZERO(_same_type_((a), &(a)[0]))
#else
#define _must_be_array_(a)		0
#endif


#define _deprecated_			__attribute__((deprecated))
#define _packed_			__attribute__((packed))
#define _weak_				__attribute__((weak))
#define _alias_(symbol)			__attribute__((alias(#symbol)))
#define _noreturn_			__attribute__((noreturn))

#define _pure_				__attribute__((pure))
#define _const_				__attribute__((const))
#define _aligned_(x)			__attribute__((aligned(x)))
#define _printf_(a, b)			__attribute__((format(printf, a, b)))
#define _scanf_(a, b)			__attribute__((format(scanf, a, b)))
#define _alloc_(...)			__attribute__((alloc_size(__VA_ARGS__)))
#define _sentinel_			__attribute__((sentinel))
#define _used_				__attribute__((used))
#define _maybe_unused_			__attribute__((unused))
#define _unused_			__attribute__((unused))
#define _must_check_			__attribute__((warn_unused_result))

#define _cold_				__attribute__((cold))
#define _hot_				__attribute__((hot))

#define _malloc_			__attribute__ ((malloc))

#define _cleanup_(x)			__attribute__((cleanup(x)))

#define _constructor_			__attribute__((constructor))
#define _destructor_			__attribute__((destructor))

#define container_of(ptr, type, member) __extension__			\
({									\
	typeof( ((type *)0)->member ) *__mptr = (ptr);			\
	(type *)( (char *)__mptr - __builtin_offsetof(type,member) );	\
})

#define const_container_of(ptr, type, member) __extension__				\
({											\
	const typeof( ((type *)0)->member ) *__mptr = (ptr);				\
	(const type *)( (const char *)__mptr - __builtin_offsetof(type,member) );	\
})

#define ARRAY_SIZE(a)			(sizeof(a) / sizeof((a)[0]) + _must_be_array_(a))
