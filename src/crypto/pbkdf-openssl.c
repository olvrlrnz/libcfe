#include <cfe/compiler.h>
#include <cfe/malloc.h>
#include <crypto/pbkdf.h>
#include <openssl/evp.h>


static int openssl_pbkdf_sha1(unsigned rounds, const void *in, size_t ilen,
                              const void *salt, size_t slen, void *out, size_t olen)
{
	return PKCS5_PBKDF2_HMAC(in, ilen, salt, slen, rounds, EVP_sha1(), olen, out) != 1;
}

static int openssl_pbkdf_sha224(unsigned rounds, const void *in, size_t ilen,
                                const void *salt, size_t slen, void *out, size_t olen)
{
	return PKCS5_PBKDF2_HMAC(in, ilen, salt, slen, rounds, EVP_sha224(), olen, out) != 1;
}

static int openssl_pbkdf_sha256(unsigned rounds, const void *in, size_t ilen,
                                const void *salt, size_t slen, void *out, size_t olen)
{
	return PKCS5_PBKDF2_HMAC(in, ilen, salt, slen, rounds, EVP_sha256(), olen, out) != 1;
}

static int openssl_pbkdf_sha384(unsigned rounds, const void *in, size_t ilen,
                                const void *salt, size_t slen, void *out, size_t olen)
{
	return PKCS5_PBKDF2_HMAC(in, ilen, salt, slen, rounds, EVP_sha384(), olen, out) != 1;
}

static int openssl_pbkdf_sha512(unsigned rounds, const void *in, size_t ilen,
                                const void *salt, size_t slen, void *out, size_t olen)
{
	return PKCS5_PBKDF2_HMAC(in, ilen, salt, slen, rounds, EVP_sha512(), olen, out) != 1;
}

static struct cfe_pbkdf_type openssl_type_kdf_sha1 = {
	.name		= "pbkdf-sha1",
	.derive		= openssl_pbkdf_sha1,
};

static struct cfe_pbkdf_type openssl_type_kdf_sha224 = {
	.name		= "pbkdf-sha224",
	.derive		= openssl_pbkdf_sha224,
};
static struct cfe_pbkdf_type openssl_type_kdf_sha256 = {
	.name		= "pbkdf-sha256",
	.derive		= openssl_pbkdf_sha256,
};
static struct cfe_pbkdf_type openssl_type_kdf_sha384 = {
	.name		= "pbkdf-sha384",
	.derive		= openssl_pbkdf_sha384,
};
static struct cfe_pbkdf_type openssl_type_kdf_sha512 = {
	.name		= "pbkdf-sha512",
	.derive		= openssl_pbkdf_sha512,
};

static void _constructor_ pbkdf_openssl_init(void)
{
	int ret;

	ret  = 0;
	ret += cfe_pbkdf_register(&openssl_type_kdf_sha1);
	ret += cfe_pbkdf_register(&openssl_type_kdf_sha224);
	ret += cfe_pbkdf_register(&openssl_type_kdf_sha256);
	ret += cfe_pbkdf_register(&openssl_type_kdf_sha384);
	ret += cfe_pbkdf_register(&openssl_type_kdf_sha512);
	if (ret)
		abort();
}
