#ifndef LIBCFE_COMPILER_H
#define LIBCFE_COMPILER_H


#define ___PASTE(a,b)			a##b
#define __PASTE(a,b)			___PASTE(a,b)

#ifdef __GNUC__
#include <cfe/compiler-gcc.h>
#endif

#ifdef __clang__
#include <cfe/compiler-clang.h>
#endif

#define likely(x)			__builtin_expect(!!(x), 1)
#define unlikely(x)			__builtin_expect(!!(x), 0)

#ifndef _used_
#define _used_
#endif

#ifndef _unused_
#define _unused_
#endif

#ifndef _hot_
#define _hot_
#endif

#ifndef _cold_
#define _cold_
#endif

#ifndef _const_
#define _const_
#endif

#ifndef _same_type_
#define _same_type_(a, b)		__builtin_types_compatible_p(typeof(a), typeof(b))
#endif


#endif
