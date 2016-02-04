#ifndef LIBCFE_CRYPTO_RAND_H
#define LIBCFE_CRYPTO_RAND_H


#include <cfe/list.h>
#include <cfe/atomic.h>


struct cfe_rand_type {
	const char *name;
	cfe_atomic_t refcnt;
	struct list_head next;

	int (*get)(void *buffer, size_t size);
};


extern int cfe_rand_register(struct cfe_rand_type *type);

extern int cfe_rand_unregister(struct cfe_rand_type *type);

extern int cfe_rand_get(void *buffer, size_t size);

#endif
