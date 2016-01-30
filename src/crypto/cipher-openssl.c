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
                                    const unsigned char *key, size_t ksize,
                                    const unsigned char *iv, size_t isize)
{
	int res;
	struct openssl_generic_ctx *octx;

	octx = OPENSSL_CTX(ctx);
	res = EVP_CipherInit_ex(&octx->ctx, octx->cipher, NULL, NULL, NULL, mode);
	if (res != 1)
		return 1;

	if (ctx->type->flags & CIPHER_TYPE_AEAD) {
		/* TODO: replace w/ EVP_CTRL_AEAD_SET_IVLEN */
		res = EVP_CIPHER_CTX_ctrl(&octx->ctx, EVP_CTRL_GCM_SET_IVLEN, isize, NULL);
		if (res != 1)
			return 1;
	}

	return EVP_CipherInit_ex(&octx->ctx, NULL, NULL, key, iv, mode) != 1;
}

static int openssl_generic_ctx_update(struct cfe_cipher_ctx *ctx,
                                      unsigned char *in, size_t inlen,
                                      unsigned char *out, size_t *olen)
{
	struct openssl_generic_ctx *octx;

	octx = OPENSSL_CTX(ctx);
	return EVP_CipherUpdate(&octx->ctx, out, (int *) olen, in, (int) inlen) != 1;
}

static int openssl_generic_ctx_finalize(struct cfe_cipher_ctx *ctx,
                                        unsigned char *out, size_t *olen)
{
	struct openssl_generic_ctx *octx;

	octx = OPENSSL_CTX(ctx);
	return EVP_CipherFinal_ex(&octx->ctx, out, (int *) olen) != 1;
}

static int openssl_generic_ctx_set_padding(struct cfe_cipher_ctx *ctx, int on)
{
	struct openssl_generic_ctx *octx;

	octx = OPENSSL_CTX(ctx);
	return EVP_CIPHER_CTX_set_padding(&octx->ctx, on ? 1 : 0) != 1;
}

static int openssl_generic_ctx_set_aad(struct cfe_cipher_ctx *ctx,
                                       unsigned char *aad, size_t asize)
{
	size_t size;
	return openssl_generic_ctx_update(ctx, aad, asize, NULL, &size);
}

static int openssl_generic_ctx_get_tag(struct cfe_cipher_ctx *ctx,
                                       unsigned char *tag, size_t tsize)
{
	struct openssl_generic_ctx *octx;

	octx = OPENSSL_CTX(ctx);
	/* TODO: replace w/ EVP_CTRL_AEAD_GET_TAG */
	return EVP_CIPHER_CTX_ctrl(&octx->ctx, EVP_CTRL_GCM_GET_TAG, tsize, tag) != 1;
}

static int openssl_generic_ctx_set_tag(struct cfe_cipher_ctx *ctx,
                                       unsigned char *tag, size_t tsize)
{
	struct openssl_generic_ctx *octx;

	octx = OPENSSL_CTX(ctx);
	/* TODO: replace w/ EVP_CTRL_AEAD_SET_TAG */
	return EVP_CIPHER_CTX_ctrl(&octx->ctx, EVP_CTRL_GCM_SET_TAG, tsize, tag) != 1;
}

static void openssl_destroy_ctx(struct cfe_cipher_ctx *ctx)
{
	struct openssl_generic_ctx *octx;

	octx = OPENSSL_CTX(ctx);
	EVP_CIPHER_CTX_cleanup(&octx->ctx);
	cfe_free(octx);
}

static const struct cfe_cipher_aead_ops openssl_generic_aead_ctx_ops = {
	.set_aad		= openssl_generic_ctx_set_aad,
	.get_tag		= openssl_generic_ctx_get_tag,
	.set_tag		= openssl_generic_ctx_set_tag,
	.base			= {
		.init		= openssl_generic_ctx_init,
		.update		= openssl_generic_ctx_update,
		.finalize	= openssl_generic_ctx_finalize,
		.set_padding	= openssl_generic_ctx_set_padding,
		.destroy	= openssl_destroy_ctx,
	}
};

static struct cfe_cipher_ctx *openssl_alloc_ctx(struct cfe_cipher_type *type,
                                                const EVP_CIPHER *cipher,
                                                const struct cfe_cipher_ops *ops)
{
	struct openssl_generic_ctx *octx;

	octx = cfe_calloc(1, sizeof(*octx));
	if (octx) {
		octx->cipher = cipher;
		octx->base.type = type;
		octx->base.ops = ops;
	}

	return &octx->base;
}

static inline struct cfe_cipher_ctx *openssl_aes_cbc128_alloc_ctx(void)
{
	return openssl_alloc_ctx(&openssl_type_aes_cbc128, EVP_aes_128_cbc(),
	                         &openssl_generic_aead_ctx_ops.base);
}

static inline struct cfe_cipher_ctx *openssl_aes_cbc192_alloc_ctx(void)
{
	return openssl_alloc_ctx(&openssl_type_aes_cbc192, EVP_aes_192_cbc(),
	                         &openssl_generic_aead_ctx_ops.base);
}

static inline struct cfe_cipher_ctx *openssl_aes_cbc256_alloc_ctx(void)
{
	return openssl_alloc_ctx(&openssl_type_aes_cbc256, EVP_aes_256_cbc(),
	                         &openssl_generic_aead_ctx_ops.base);
}

static inline struct cfe_cipher_ctx *openssl_aes_gcm128_alloc_ctx(void)
{
	return openssl_alloc_ctx(&openssl_type_aes_gcm128, EVP_aes_128_gcm(),
	                         &openssl_generic_aead_ctx_ops.base);
}

static inline struct cfe_cipher_ctx *openssl_aes_gcm192_alloc_ctx(void)
{
	return openssl_alloc_ctx(&openssl_type_aes_gcm192, EVP_aes_192_gcm(),
	                         &openssl_generic_aead_ctx_ops.base);
}

static inline struct cfe_cipher_ctx *openssl_aes_gcm256_alloc_ctx(void)
{
	return openssl_alloc_ctx(&openssl_type_aes_gcm256, EVP_aes_256_gcm(),
	                         &openssl_generic_aead_ctx_ops.base);
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
	.flags		= CIPHER_TYPE_AEAD,
	.ivsize		= 16,
	.keysize	= 16,
	.blocksize	= 1,
	.alloc_ctx	= openssl_aes_gcm128_alloc_ctx,
};

static struct cfe_cipher_type openssl_type_aes_gcm192 = {
	.name		= "aes-gcm-192",
	.flags		= CIPHER_TYPE_AEAD,
	.ivsize		= 16,
	.keysize	= 24,
	.blocksize	= 1,
	.alloc_ctx	= openssl_aes_gcm192_alloc_ctx,
};

static struct cfe_cipher_type openssl_type_aes_gcm256 = {
	.name		= "aes-gcm-256",
	.flags		= CIPHER_TYPE_AEAD,
	.ivsize		= 16,
	.keysize	= 32,
	.blocksize	= 1,
	.alloc_ctx	= openssl_aes_gcm256_alloc_ctx,
};

static void _constructor_ cipher_openssl_init(void)
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
