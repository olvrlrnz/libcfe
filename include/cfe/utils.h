#ifndef LIBCFE_CFE_UTILS_H
#define LIBCFE_CFE_UTILS_H


#include <cfe/compiler.h>


static inline _const_ uint16_t next_pow2_16(uint16_t num)
{
	return num <= 2 ? num : 1U << (16 - __builtin_clz(num - 1));
}

static inline _const_ uint32_t next_pow2_32(uint32_t num)
{
	return num <= 2 ? num : 1U << (32 - __builtin_clz(num - 1));
}

static inline _const_ uint64_t next_pow2_64(uint64_t num)
{
	return num <= 2 ? num : 1ULL << (64 - __builtin_clzll(num - 1));
}


#endif
