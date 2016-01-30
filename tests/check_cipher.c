#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <cmocka.h>
#include <cfe/compiler.h>
#include <cfe/bug.h>
#include <crypto/cipher.h>
#include "utils.h"


struct test_data {
	const char *iv;
	const char *key;
	const char *plain;
	const char *cipher;
};

struct aead_test_data {
	const char *aad;
	const char *tag;
	struct test_data data;
};


static const char *zeroed_string16 = "00000000000000000000000000000000";
static const char *zeroed_string12 = "000000000000000000000000";


static void _generic_check_aead_encrypt(const char *algname,
                                        const struct aead_test_data *data)
{
	int res;
	size_t len = 0;
	size_t have = 0;
	struct cfe_cipher_ctx *ctx;

	size_t aadsize = data->aad ? strlen(data->aad) / 2 : 0;
	size_t tagsize = strlen(data->tag) / 2;
	size_t ivsize = strlen(data->data.iv) / 2;
	size_t keysize = strlen(data->data.key) / 2;
	size_t plainsize = strlen(data->data.plain) / 2;
	size_t ciphersize = strlen(data->data.cipher) / 2;

	unsigned char _aad[128];
	unsigned char _tag[128];
	unsigned char _iv[128];
	unsigned char _key[128];
	unsigned char _plain[128];
	unsigned char _cipher[128];
	unsigned char _result[128];
	unsigned char _tagresult[128];

	if (data->aad)
		hex_str_to_bytes(data->aad, _aad);
	hex_str_to_bytes(data->tag, _tag);
	hex_str_to_bytes(data->data.iv, _iv);
	hex_str_to_bytes(data->data.key, _key);
	hex_str_to_bytes(data->data.plain, _plain);
	hex_str_to_bytes(data->data.cipher, _cipher);

	ctx = cfe_cipher_alloc_ctx(algname);
	assert_ptr_not_equal(ctx, NULL);

	res = cfe_cipher_ctx_init(ctx, CFE_CIPHER_MODE_ENCRYPT, _key, keysize, _iv, ivsize);
	assert_int_equal(res, 0);

	res = cfe_cipher_ctx_set_padding(ctx, 0);
	assert_int_equal(res, 0);

	if (data->aad) {
		res = cfe_cipher_ctx_set_aad(ctx, _aad, aadsize);
		assert_int_equal(res, 0);
	}

	res = cfe_cipher_ctx_update(ctx, _plain, plainsize, _result, &have);
	assert_int_equal(have, plainsize);
	assert_int_equal(res, 0);

	len += have;
	res = cfe_cipher_ctx_finalize(ctx, _result + len, &have);
	assert_int_equal(have, 0);
	assert_int_equal(res, 0);

	len += have;
	res = cfe_cipher_ctx_get_tag(ctx, _tagresult, tagsize);
	assert_int_equal(res, 0);

	res = memcmp(_result, _cipher, len);
	assert_int_equal(res, 0);

	res = memcmp(_tagresult, _tag, tagsize);
	assert_int_equal(res, 0);

	cfe_cipher_destroy_ctx(ctx);
}

static void _generic_check_aead_decrypt(const char *algname,
                                        const struct aead_test_data *data)
{
	int res;
	size_t len = 0;
	size_t have = 0;
	struct cfe_cipher_ctx *ctx;

	size_t aadsize = data->aad ? strlen(data->aad) / 2 : 0;
	size_t tagsize = strlen(data->tag) / 2;
	size_t ivsize = strlen(data->data.iv) / 2;
	size_t keysize = strlen(data->data.key) / 2;
	size_t plainsize = strlen(data->data.plain) / 2;
	size_t ciphersize = strlen(data->data.cipher) / 2;

	unsigned char _aad[128];
	unsigned char _tag[128];
	unsigned char _iv[128];
	unsigned char _key[128];
	unsigned char _plain[128];
	unsigned char _cipher[128];
	unsigned char _result[128];
	unsigned char _tagresult[128];

	if (data->aad)
		hex_str_to_bytes(data->aad, _aad);
	hex_str_to_bytes(data->tag, _tag);
	hex_str_to_bytes(data->data.iv, _iv);
	hex_str_to_bytes(data->data.key, _key);
	hex_str_to_bytes(data->data.plain, _plain);
	hex_str_to_bytes(data->data.cipher, _cipher);

	ctx = cfe_cipher_alloc_ctx(algname);
	assert_ptr_not_equal(ctx, NULL);

	res = cfe_cipher_ctx_init(ctx, CFE_CIPHER_MODE_DECRYPT, _key, keysize, _iv, ivsize);
	assert_int_equal(res, 0);

	res = cfe_cipher_ctx_set_padding(ctx, 0);
	assert_int_equal(res, 0);

	if (data->aad) {
		res = cfe_cipher_ctx_set_aad(ctx, _aad, aadsize);
		assert_int_equal(res, 0);
	}

	res = cfe_cipher_ctx_update(ctx, _cipher, ciphersize, _result, &have);
	assert_int_equal(have, plainsize);
	assert_int_equal(res, 0);

	len += have;

	res = cfe_cipher_ctx_set_tag(ctx, _tag, tagsize);
	assert_int_equal(res, 0);

	res = cfe_cipher_ctx_finalize(ctx, _result + len, &have);
	assert_int_equal(have, 0);
	assert_int_equal(res, 0);

	res = memcmp(_result, _plain, have);
	assert_int_equal(res, 0);

	cfe_cipher_destroy_ctx(ctx);
}

static void _generic_check_aead_cipher(const char *algname,
                                       const struct aead_test_data *data)
{
	_generic_check_aead_encrypt(algname, data);
	_generic_check_aead_decrypt(algname, data);
}

static void generic_check_aead_cipher(const char *algname,
                                      const struct aead_test_data *data,
                                      size_t dsize)
{
	size_t i;

	for (i = 0; i < dsize; ++i)
		_generic_check_aead_cipher(algname, data + i);
}


static void __generic_check_cipher(const char *algname,
                                   enum cfe_cipher_mode mode,
                                   const char *iv, const char *key,
                                   const char *plain, const char *cipher)
{
	int res;
	size_t have = 0;
	struct cfe_cipher_ctx *ctx;

	size_t ivsize = strlen(iv) / 2;
	size_t keysize = strlen(key) / 2;
	size_t plainsize = strlen(plain) / 2;
	size_t ciphersize = strlen(cipher) / 2;

	unsigned char _iv[ivsize];
	unsigned char _key[keysize];
	unsigned char _plain[plainsize];
	unsigned char _cipher[ciphersize];
	unsigned char _result[ciphersize];

	hex_str_to_bytes(iv, _iv);
	hex_str_to_bytes(key, _key);
	hex_str_to_bytes(plain, _plain);
	hex_str_to_bytes(cipher, _cipher);

	ctx = cfe_cipher_alloc_ctx(algname);
	assert_ptr_not_equal(ctx, NULL);

	res = cfe_cipher_ctx_init(ctx, mode, _key, keysize, _iv, ivsize);
	assert_int_equal(res, 0);

	res = cfe_cipher_ctx_set_padding(ctx, 0);
	assert_int_equal(res, 0);

	res = cfe_cipher_ctx_update(ctx, _plain, plainsize, _result, &have);
	assert_int_equal(have, plainsize);
	assert_int_equal(res, 0);

	res = cfe_cipher_ctx_finalize(ctx, _result + have, &have);
	assert_int_equal(have, 0);
	assert_int_equal(res, 0);

	res = memcmp(_result, _cipher, have);
	assert_int_equal(res, 0);

	cfe_cipher_destroy_ctx(ctx);
}

static void _generic_check_cipher(const char *algname,
                                  const struct test_data *data)
{
	__generic_check_cipher(algname, CFE_CIPHER_MODE_ENCRYPT,
	                       data->iv, data->key, data->plain, data->cipher);

	__generic_check_cipher(algname, CFE_CIPHER_MODE_DECRYPT,
	                       data->iv, data->key, data->cipher, data->plain);
}

static void generic_check_cipher(const char *algname,
                                 const struct test_data *data, size_t dsize)
{
	size_t i;

	for (i = 0; i < dsize; ++i)
		_generic_check_cipher(algname, data + i);
}

static void check_cbc128(void **state)
{
	const struct test_data data[] = {
		{
			.iv	= zeroed_string16,
			.key	= "2b7e151628aed2a6abf7158809cf4f3c",
			.plain	= "6bc1bee22e409f96e93d7e117393172a",
			.cipher	= "3ad77bb40d7a3660a89ecaf32466ef97",
		}, {
			.iv	= zeroed_string16,
			.key	= "2b7e151628aed2a6abf7158809cf4f3c",
			.plain	= "ae2d8a571e03ac9c9eb76fac45af8e51",
			.cipher	= "f5d3d58503b9699de785895a96fdbaaf",
		}, {
			.iv	= zeroed_string16,
			.key	= "2b7e151628aed2a6abf7158809cf4f3c",
			.plain	= "30c81c46a35ce411e5fbc1191a0a52ef",
			.cipher	= "43b1cd7f598ece23881b00e3ed030688",
		}, {
			.iv	= zeroed_string16,
			.key	= "2b7e151628aed2a6abf7158809cf4f3c",
			.plain	= "f69f2445df4f9b17ad2b417be66c3710",
			.cipher	= "7b0c785e27e8ad3f8223207104725dd4",
		}
	};

	generic_check_cipher("aes-cbc-128", data, ARRAY_SIZE(data));
}

static void check_cbc192(void **state)
{
	const struct test_data data[] = {
		{
			.iv	= zeroed_string16,
			.key	= "8e73b0f7da0e6452c810f32b809079e562f8ead2522c6b7b",
			.plain	= "6bc1bee22e409f96e93d7e117393172a",
			.cipher	= "bd334f1d6e45f25ff712a214571fa5cc",
		}, {
			.iv	= zeroed_string16,
			.key	= "8e73b0f7da0e6452c810f32b809079e562f8ead2522c6b7b",
			.plain	= "ae2d8a571e03ac9c9eb76fac45af8e51",
			.cipher	= "974104846d0ad3ad7734ecb3ecee4eef",
		}, {
			.iv	= zeroed_string16,
			.key	= "8e73b0f7da0e6452c810f32b809079e562f8ead2522c6b7b",
			.plain	= "30c81c46a35ce411e5fbc1191a0a52ef",
			.cipher	= "ef7afd2270e2e60adce0ba2face6444e",
		}, {
			.iv	= zeroed_string16,
			.key	= "8e73b0f7da0e6452c810f32b809079e562f8ead2522c6b7b",
			.plain	= "f69f2445df4f9b17ad2b417be66c3710",
			.cipher	= "9a4b41ba738d6c72fb16691603c18e0e",
		}
	};

	generic_check_cipher("aes-cbc-192", data, ARRAY_SIZE(data));
}

static void check_cbc256(void **state)
{
	const struct test_data data[] = {
		{
			.iv	= "000102030405060708090A0B0C0D0E0F",
			.key	= "603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4",
			.plain	= "6bc1bee22e409f96e93d7e117393172a",
			.cipher	= "f58c4c04d6e5f1ba779eabfb5f7bfbd6",
		}, {
			.iv	= "F58C4C04D6E5F1BA779EABFB5F7BFBD6",
			.key	= "603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4",
			.plain	= "ae2d8a571e03ac9c9eb76fac45af8e51",
			.cipher	= "9cfc4e967edb808d679f777bc6702c7d",
		}, {
			.iv	= "9CFC4E967EDB808D679F777BC6702C7D",
			.key	= "603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4",
			.plain	= "30c81c46a35ce411e5fbc1191a0a52ef",
			.cipher	= "39f23369a9d9bacfa530e26304231461",
		}, {
			.iv	= "39F23369A9D9BACFA530E26304231461",
			.key	= "603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4",
			.plain	= "f69f2445df4f9b17ad2b417be66c3710",
			.cipher	= "b2eb05e2c39be9fcda6c19078c6a9d1b",
		}
	};

	generic_check_cipher("aes-cbc-256", data, ARRAY_SIZE(data));
}

static void check_gcm128(void **state)
{
	const struct aead_test_data data[] = {
		{
			.aad	= "feedfacedeadbeeffeedfacedeadbeef"
			          "abaddad2",
			.tag	= "5bc94fbc3221a5db94fae95ae7121a47",
			.data	= {
			.iv	= "cafebabefacedbaddecaf888",
			.key	= "feffe9928665731c6d6a8f9467308308",
			.plain	= "d9313225f88406e5a55909c5aff5269a"
			          "86a7a9531534f7da2e4c303d8a318a72"
			          "1c3c0c95956809532fcf0e2449a6b525"
			          "b16aedf5aa0de657ba637b39",
			.cipher	= "42831ec2217774244b7221b784d0d49c"
			          "e3aa212f2c02a4e035c17e2329aca12e"
			          "21d514b25466931c7d8f6a5aac84aa05"
			          "1ba30b396a0aac973d58e091",
			}
		}
	};

	generic_check_aead_cipher("aes-gcm-128", data, ARRAY_SIZE(data));
}


int main(int argc, char *argv[])
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(check_cbc128),
		cmocka_unit_test(check_cbc192),
		cmocka_unit_test(check_cbc256),
		cmocka_unit_test(check_gcm128),
	};

	unlink(__FILE__".xml");
	setenv("CMOCKA_XML_FILE", __FILE__".xml", 1);
	cmocka_set_message_output(CM_OUTPUT_XML);
	return cmocka_run_group_tests_name("cipher", tests, NULL, NULL);
}
