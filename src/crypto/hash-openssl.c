#include <cfe/compiler.h>
#include <cfe/malloc.h>
#include <crypto/hash.h>
#include <openssl/evp.h>


static struct cfe_hash_type openssl_type_sha1;
static struct cfe_hash_type openssl_type_sha224;
static struct cfe_hash_type openssl_type_sha256;
static struct cfe_hash_type openssl_type_sha384;
static struct cfe_hash_type openssl_type_sha512;


struct openssl_generic_ctx {
	const EVP_MD *md;
	EVP_MD_CTX mdctx;
	struct cfe_hash_ctx base;
};


static inline struct openssl_generic_ctx *OPENSSL_CTX(struct cfe_hash_ctx *ctx)
{
	return container_of(ctx, struct openssl_generic_ctx, base);
}


static int openssl_generic_ctx_init(struct cfe_hash_ctx *ctx)
{
	struct openssl_generic_ctx *octx;

	octx = OPENSSL_CTX(ctx);
	return EVP_DigestInit_ex(&octx->mdctx, octx->md, NULL) == 0;
}

static int openssl_generic_ctx_update(struct cfe_hash_ctx *ctx,
                                      const void *buffer, size_t size)
{
	struct openssl_generic_ctx *octx;

	octx = OPENSSL_CTX(ctx);
	return EVP_DigestUpdate(&octx->mdctx, buffer, size) == 0;
}

static int openssl_generic_ctx_finalize(struct cfe_hash_ctx *ctx, void *buffer)
{
	struct openssl_generic_ctx *octx;

	octx = OPENSSL_CTX(ctx);
	return EVP_DigestFinal(&octx->mdctx, buffer, NULL) == 0;
}

static void openssl_destroy_ctx(struct cfe_hash_ctx *ctx)
{
	struct openssl_generic_ctx *octx;

	octx = OPENSSL_CTX(ctx);
	EVP_MD_CTX_cleanup(&octx->mdctx);
	cfe_free(octx);
}

static const struct cfe_hash_ops openssl_generic_ctx_ops = {
	.init		= openssl_generic_ctx_init,
	.update		= openssl_generic_ctx_update,
	.finalize	= openssl_generic_ctx_finalize,
	.destroy	= openssl_destroy_ctx,
};


static struct cfe_hash_ctx *openssl_alloc_ctx(struct cfe_hash_type *type,
                                              const EVP_MD *md)
{
	struct openssl_generic_ctx *octx;

	octx = cfe_calloc(1, sizeof(*octx));
	if (octx) {
		octx->md = md;
		octx->base.type = type;
		octx->base.ops = &openssl_generic_ctx_ops;
	}

	return &octx->base;
}

static inline struct cfe_hash_ctx *openssl_sha1_alloc_ctx(void)
{
	return openssl_alloc_ctx(&openssl_type_sha1, EVP_sha1());
}

static inline struct cfe_hash_ctx *openssl_sha224_alloc_ctx(void)
{
	return openssl_alloc_ctx(&openssl_type_sha224, EVP_sha224());
}

static inline struct cfe_hash_ctx *openssl_sha256_alloc_ctx(void)
{
	return openssl_alloc_ctx(&openssl_type_sha256, EVP_sha256());
}

static inline struct cfe_hash_ctx *openssl_sha384_alloc_ctx(void)
{
	return openssl_alloc_ctx(&openssl_type_sha384, EVP_sha384());
}

static inline struct cfe_hash_ctx *openssl_sha512_alloc_ctx(void)
{
	return openssl_alloc_ctx(&openssl_type_sha512, EVP_sha512());
}

static struct cfe_hash_type openssl_type_sha1 = {
	.name		= "sha1",
	.digestsize	= 20,
	.alloc_ctx	= openssl_sha1_alloc_ctx,
};

static struct cfe_hash_type openssl_type_sha224 = {
	.name		= "sha224",
	.digestsize	= 28,
	.alloc_ctx	= openssl_sha224_alloc_ctx,
};

static struct cfe_hash_type openssl_type_sha256 = {
	.name		= "sha256",
	.digestsize	= 32,
	.alloc_ctx	= openssl_sha256_alloc_ctx,
};

static struct cfe_hash_type openssl_type_sha384 = {
	.name		= "sha384",
	.digestsize	= 48,
	.alloc_ctx	= openssl_sha384_alloc_ctx,
};

static struct cfe_hash_type openssl_type_sha512 = {
	.name		= "sha512",
	.digestsize	= 64,
	.alloc_ctx	= openssl_sha512_alloc_ctx,
};

static void _constructor_ hash_openssl_init(void)
{
	int ret;

	ret  = 0;
	ret += cfe_hash_register(&openssl_type_sha1);
	ret += cfe_hash_register(&openssl_type_sha224);
	ret += cfe_hash_register(&openssl_type_sha256);
	ret += cfe_hash_register(&openssl_type_sha384);
	ret += cfe_hash_register(&openssl_type_sha512);

	/* TODO: report error */
}
