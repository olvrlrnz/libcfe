#ifndef LIBCFE_CFE_MALLOC_H
#define LIBCFE_CFE_MALLOC_H


#include <stdlib.h>


static inline void *cfe_malloc(size_t size)
{
	return malloc(size);
}

static inline void *cfe_calloc(size_t nmemb, size_t size)
{
	return calloc(nmemb, size);
}

static inline void cfe_free(void *ptr)
{
	free(ptr);
}

static inline void *cfe_realloc(void *ptr, size_t size)
{
	return realloc(ptr, size);
}



static inline void *cfe_malloc_secure(size_t size)
{
	/* TODO: implement me */
	return cfe_malloc(size);
}

static inline void *cfe_calloc_secure(size_t nmemb, size_t size)
{
	/* TODO: implement me */
	return cfe_calloc(nmemb, size);
}

static inline void cfe_free_secure(void *ptr)
{
	/* TODO: implement me */
	cfe_free(ptr);
}

static inline void *cfe_realloc_secure(void *ptr, size_t size)
{
	/* TODO: implement me */
	return cfe_realloc(ptr, size);
}


#endif
