#ifndef LIBCFE_CFE_REF_H
#define LIBCFE_CFE_REF_H


#include <cfe/atomic.h>


struct cfe_ref {
	cfe_atomic_t refcount;
};


static inline void cfe_ref_init(struct cfe_ref *ref)
{
	cfe_atomic_set(&ref->refcount, 1);
}

static inline void cfe_ref_get(struct cfe_ref *ref)
{
	cfe_atomic_inc(&ref->refcount);
}

static inline int cfe_ref_sub(struct cfe_ref *ref, int count,
                              void (*release)(struct cfe_ref *ref))
{
	if (cfe_atomic_sub_and_test(&ref->refcount, count)) {
		release(ref);
		return 1;
	}

	return 0;
}

static inline int cfe_ref_put(struct cfe_ref *ref,
                              void (*release)(struct cfe_ref *ref))
{
	return cfe_ref_sub(ref, 1, release);
}

#endif
