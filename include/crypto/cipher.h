#ifndef LIBCFE_CRYPTO_CIPHER_H
#define LIBCFE_CRYPTO_CIPHER_H


#include <cfe/list.h>
#include <cfe/atomic.h>
#include <cfe/compiler.h>


#define CIPHERNAME_LEN			16U


struct cfe_cipher_ctx;
struct cfe_cipher_type;


enum cfe_cipher_mode {
	CFE_CIPHER_MODE_DECRYPT = 0,
	CFE_CIPHER_MODE_ENCRYPT = 1,
};


#define CIPHER_TYPE_AEAD		0x00000001


struct cfe_cipher_ops {
	int (*init)(struct cfe_cipher_ctx *ctx, enum cfe_cipher_mode mode,
	            const unsigned char *key, size_t ksize,
	            const unsigned char *iv, size_t isize);
	int (*update)(struct cfe_cipher_ctx *ctx,
	              unsigned char *in, size_t ilen,
	              unsigned char *out, size_t *olen);
	int (*finalize)(struct cfe_cipher_ctx *ctx,
	                unsigned char *out, size_t *olen);

	void (*destroy)(struct cfe_cipher_ctx *ctx);

	int (*set_padding)(struct cfe_cipher_ctx *ctx, int on);
};

struct cfe_cipher_aead_ops {
	int (*set_aad)(struct cfe_cipher_ctx *ctx,
	               unsigned char *aad, size_t asize);
	int (*get_tag)(struct cfe_cipher_ctx *ctx,
	               unsigned char *tag, size_t tsize);
	int (*set_tag)(struct cfe_cipher_ctx *ctx,
	               unsigned char *tag, size_t tsize);

	struct cfe_cipher_ops base;
};


static inline struct cfe_cipher_aead_ops *AEAD_OPS(struct cfe_cipher_ops *ops)
{
	return container_of(ops, struct cfe_cipher_aead_ops, base);
}


struct cfe_cipher_ctx {
	const struct cfe_cipher_ops *ops;
	struct cfe_cipher_type *type;
};

struct cfe_cipher_type {
	char name[CIPHERNAME_LEN];
	int flags;
	size_t ivsize;
	size_t keysize;
	unsigned blocksize;
	cfe_atomic_t refcnt;
	struct list_head next;

	struct cfe_cipher_ctx *(*alloc_ctx)(void);
};


extern int cfe_cipher_register(struct cfe_cipher_type *type);

extern int cfe_cipher_unregister(struct cfe_cipher_type *type);

extern struct cfe_cipher_ctx *cfe_cipher_alloc_ctx(const char *algname);

extern void cfe_cipher_destroy_ctx(struct cfe_cipher_ctx *ctx);


extern int cfe_cipher_ctx_blocksize(struct cfe_cipher_ctx *ctx);

extern int cfe_cipher_ctx_init(struct cfe_cipher_ctx *ctx,
                               enum cfe_cipher_mode mode,
                               const unsigned char *key, size_t ksize,
                               const unsigned char *iv, size_t isize);

extern int cfe_cipher_ctx_update(struct cfe_cipher_ctx *ctx,
                                 unsigned char *in, size_t ilen,
                                 unsigned char *out, size_t *olen);

extern int cfe_cipher_ctx_finalize(struct cfe_cipher_ctx *ctx,
                                   unsigned char *out, size_t *olen);

extern int cfe_cipher_ctx_set_padding(struct cfe_cipher_ctx *ctx, int on);

extern int cfe_cipher_ctx_set_aad(struct cfe_cipher_ctx *ctx,
                                  unsigned char *aad, size_t asize);
extern int cfe_cipher_ctx_set_tag(struct cfe_cipher_ctx *ctx,
                                  unsigned char *tag, size_t tsize);
extern int cfe_cipher_ctx_get_tag(struct cfe_cipher_ctx *ctx,
                                  unsigned char *tag, size_t tsize);
#endif
