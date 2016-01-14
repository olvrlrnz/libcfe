#ifndef LIBCFE_CRYPTO_HASH_H
#define LIBCFE_CRYPTO_HASH_H


#include <cfe/list.h>
#include <cfe/atomic.h>


struct cfe_hash_ctx;
struct cfe_hash_type;


struct cfe_hash_ops {
	int (*init)(struct cfe_hash_ctx *ctx);
	int (*update)(struct cfe_hash_ctx *ctx, const void *buffer, size_t size);
	int (*finalize)(struct cfe_hash_ctx *ctx, void *buffer);
	void (*destroy)(struct cfe_hash_ctx *ctx);
};

struct cfe_hash_ctx {
	const struct cfe_hash_ops *ops;
	struct cfe_hash_type *type;
};

struct cfe_hash_type {
	const char *name;
	unsigned digestsize;
	cfe_atomic_t refcnt;
	struct list_head next;

	struct cfe_hash_ctx *(*alloc_ctx)(void);
};


extern int cfe_hash_register(struct cfe_hash_type *type);

extern int cfe_hash_unregister(struct cfe_hash_type *type);

extern struct cfe_hash_ctx *cfe_hash_alloc_ctx(const char *algname);

extern void cfe_hash_destroy_ctx(struct cfe_hash_ctx *ctx);


extern int cfe_hash_ctx_digestsize(struct cfe_hash_ctx *ctx);

extern int cfe_hash_ctx_init(struct cfe_hash_ctx *ctx);

extern int cfe_hash_ctx_update(struct cfe_hash_ctx *ctx, const void *buffer, size_t size);

extern int cfe_hash_ctx_finalize(struct cfe_hash_ctx *ctx, void *buffer);


#endif
