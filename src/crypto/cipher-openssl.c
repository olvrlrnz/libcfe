#include <cfe/compiler.h>
#include <cfe/malloc.h>
#include <crypto/cipher.h>
#include <openssl/evp.h>


static struct cfe_cipher_type openssl_type_aes_cbc128;
static struct cfe_cipher_type openssl_type_aes_cbc192;
static struct cfe_cipher_type openssl_type_aes_cbc256;
static struct cfe_cipher_type openssl_type_aes_gcm128;
static struct cfe_cipher_type openssl_type_aes_gcm192;
static struct cfe_cipher_type openssl_type_aes_gcm256;


struct openssl_generic_ctx {
	const EVP_CIPHER *cipher;
	EVP_CIPHER_CTX ctx;
	struct cfe_cipher_ctx base;
};


static inline struct openssl_generic_ctx *OPENSSL_CTX(struct cfe_cipher_ctx *ctx)
{
	return container_of(ctx, struct openssl_generic_ctx, base);
}


static int openssl_generic_ctx_init(struct cfe_cipher_ctx *ctx,
                                    enum cfe_cipher_mode mode,
                                    const unsigned char *key,
                                    const unsigned char *iv)
{
	struct openssl_generic_ctx *octx;

	octx = OPENSSL_CTX(ctx);
	return EVP_CipherInit_ex(&octx->ctx, octx->cipher, NULL, key, iv, mode) == 0;
}

static int openssl_generic_ctx_update(struct cfe_cipher_ctx *ctx,
                                      unsigned char *in, size_t inlen,
                                      unsigned char *out, size_t *olen)
{
	struct openssl_generic_ctx *octx;

	octx = OPENSSL_CTX(ctx);
	return EVP_CipherUpdate(&octx->ctx,
	                        out, (int *) olen,
	                        in, (int) inlen) == 0;
}

static int openssl_generic_ctx_finalize(struct cfe_cipher_ctx *ctx,
                                        unsigned char *out, size_t *olen)
{
	struct openssl_generic_ctx *octx;

	octx = OPENSSL_CTX(ctx);
	return EVP_CipherFinal_ex(&octx->ctx, out, (int *) olen) == 0;
}

static int openssl_generic_set_padding(struct cfe_cipher_ctx *ctx, int on)
{
	struct openssl_generic_ctx *octx;

	octx = OPENSSL_CTX(ctx);
	return EVP_CIPHER_CTX_set_padding(&octx->ctx, on ? 1 : 0) == 0;
}

static void openssl_destroy_ctx(struct cfe_cipher_ctx *ctx)
{
	struct openssl_generic_ctx *octx;

	octx = OPENSSL_CTX(ctx);
	EVP_CIPHER_CTX_cleanup(&octx->ctx);
	cfe_free(octx);
}

static const struct cfe_cipher_ops openssl_generic_ctx_ops = {
	.init		= openssl_generic_ctx_init,
	.update		= openssl_generic_ctx_update,
	.finalize	= openssl_generic_ctx_finalize,
	.set_padding	= openssl_generic_set_padding,
	.destroy	= openssl_destroy_ctx,

};


static struct cfe_cipher_ctx *openssl_alloc_ctx(struct cfe_cipher_type *type,
                                                const EVP_CIPHER *cipher)
{
	struct openssl_generic_ctx *octx;

	octx = cfe_calloc(1, sizeof(*octx));
	if (octx) {
		octx->cipher = cipher;
		octx->base.type = type;
		octx->base.ops = &openssl_generic_ctx_ops;
	}

	return &octx->base;
}

static inline struct cfe_cipher_ctx *openssl_aes_cbc128_alloc_ctx(void)
{
	return openssl_alloc_ctx(&openssl_type_aes_cbc128, EVP_aes_128_cbc());
}

static inline struct cfe_cipher_ctx *openssl_aes_cbc192_alloc_ctx(void)
{
	return openssl_alloc_ctx(&openssl_type_aes_cbc192, EVP_aes_192_cbc());
}

static inline struct cfe_cipher_ctx *openssl_aes_cbc256_alloc_ctx(void)
{
	return openssl_alloc_ctx(&openssl_type_aes_cbc256, EVP_aes_256_cbc());
}

static inline struct cfe_cipher_ctx *openssl_aes_gcm128_alloc_ctx(void)
{
	return openssl_alloc_ctx(&openssl_type_aes_gcm128, EVP_aes_128_gcm());
}

static inline struct cfe_cipher_ctx *openssl_aes_gcm192_alloc_ctx(void)
{
	return openssl_alloc_ctx(&openssl_type_aes_gcm192, EVP_aes_192_gcm());
}

static inline struct cfe_cipher_ctx *openssl_aes_gcm256_alloc_ctx(void)
{
	return openssl_alloc_ctx(&openssl_type_aes_gcm256, EVP_aes_256_gcm());
}

static struct cfe_cipher_type openssl_type_aes_cbc128 = {
	.name		= "aes-cbc-128",
	.ivsize		= 16,
	.keysize	= 16,
	.blocksize	= 16,
	.alloc_ctx	= openssl_aes_cbc128_alloc_ctx,
};

static struct cfe_cipher_type openssl_type_aes_cbc192 = {
	.name		= "aes-cbc-192",
	.ivsize		= 16,
	.keysize	= 24,
	.blocksize	= 16,
	.alloc_ctx	= openssl_aes_cbc192_alloc_ctx,
};

static struct cfe_cipher_type openssl_type_aes_cbc256 = {
	.name		= "aes-cbc-256",
	.ivsize		= 16,
	.keysize	= 32,
	.blocksize	= 16,
	.alloc_ctx	= openssl_aes_cbc256_alloc_ctx,
};

static struct cfe_cipher_type openssl_type_aes_gcm128 = {
	.name		= "aes-gcm-128",
	.ivsize		= 16,
	.keysize	= 16,
	.blocksize	= 1,
	.alloc_ctx	= openssl_aes_gcm128_alloc_ctx,
};

static struct cfe_cipher_type openssl_type_aes_gcm192 = {
	.name		= "aes-gcm-192",
	.ivsize		= 16,
	.keysize	= 24,
	.blocksize	= 1,
	.alloc_ctx	= openssl_aes_gcm192_alloc_ctx,
};

static struct cfe_cipher_type openssl_type_aes_gcm256 = {
	.name		= "aes-gcm-256",
	.ivsize		= 16,
	.keysize	= 32,
	.blocksize	= 1,
	.alloc_ctx	= openssl_aes_gcm256_alloc_ctx,
};

static void _constructor_ hash_openssl_init(void)
{
	int ret;

	ret  = 0;
	ret += cfe_cipher_register(&openssl_type_aes_cbc128);
	ret += cfe_cipher_register(&openssl_type_aes_cbc192);
	ret += cfe_cipher_register(&openssl_type_aes_cbc256);
	ret += cfe_cipher_register(&openssl_type_aes_gcm128);
	ret += cfe_cipher_register(&openssl_type_aes_gcm192);
	ret += cfe_cipher_register(&openssl_type_aes_gcm256);

	if (ret)
		abort();
}
