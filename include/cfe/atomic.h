#ifndef LIBCFE_CFE_ATOMIC_H
#define LIBCFE_CFE_ATOMIC_H


typedef struct {
	long counter;
} cfe_atomic_t;


static inline void cfe_atomic_set(cfe_atomic_t *v, int val)
{
	__atomic_store_n(&v->counter, val, __ATOMIC_SEQ_CST);
}

static inline int cfe_atomic_get(cfe_atomic_t *v)
{
	return __atomic_load_n(&v->counter, __ATOMIC_SEQ_CST);
}

static inline void cfe_atomic_add(cfe_atomic_t *v, int count)
{
	__atomic_add_fetch(&v->counter, count, __ATOMIC_SEQ_CST);
}

static inline void cfe_atomic_sub(cfe_atomic_t *v, int count)
{
	__atomic_sub_fetch(&v->counter, count, __ATOMIC_SEQ_CST);
}

static inline void cfe_atomic_inc(cfe_atomic_t *v)
{
	cfe_atomic_add(v, 1);
}

static inline void cfe_atomic_dec(cfe_atomic_t *v)
{
	cfe_atomic_sub(v, 1);
}

static inline int cfe_atomic_sub_and_test(cfe_atomic_t *v, int count)
{
	return (__atomic_sub_fetch(&v->counter, count, __ATOMIC_SEQ_CST) == 0);
}
#endif
