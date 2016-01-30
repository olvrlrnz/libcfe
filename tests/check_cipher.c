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


struct test_data {
	const char *iv;
	const char *key;
	const char *plain;
	const char *cipher;
};

static void fromhex(const unsigned char *in, size_t isize, char *out)
{
	size_t i;
	char *tmp;

	for (i = 0, tmp = out; i < isize; ++i)
		tmp += sprintf(tmp, "%02x", in[i]);

	*tmp = '\0';
}

static void tohex(const char *in, unsigned char *out)
{
	size_t i, len;
	const char *tmp;

	len = strlen(in);
	for (i = 0, tmp = in; i < len; ++i) {
		sscanf(tmp, "%2hhx", &out[i]);
		tmp += 2;
	}
}

static void __generic_check_cipher(const char *algname,
                                   enum cfe_cipher_mode mode,
                                   const char *iv, const char *key,
                                   const char *plain, const char *cipher)
{
	int res;
	size_t have = 0;
	struct cfe_cipher_ctx *ctx;

	size_t ivsize = iv ? strlen(iv) / 2: 0;
	size_t keysize = strlen(key) / 2;
	size_t plainsize = strlen(plain) / 2;
	size_t ciphersize = strlen(cipher)/ 2;

	unsigned char _iv[ivsize];
	unsigned char _key[keysize];
	unsigned char _plain[plainsize];
	unsigned char _cipher[ciphersize];
	unsigned char _result[2 * ciphersize];

	if (iv)
		tohex(iv, _iv);
	tohex(key, _key);
	tohex(plain, _plain);
	tohex(cipher, _cipher);

	ctx = cfe_cipher_alloc_ctx(algname);
	assert_ptr_not_equal(ctx, NULL);

	res = cfe_cipher_ctx_init(ctx, mode, _key, keysize, iv ? _iv : NULL, ivsize);
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
			.iv	= NULL,
			.key	= "2b7e151628aed2a6abf7158809cf4f3c",
			.plain	= "6bc1bee22e409f96e93d7e117393172a",
			.cipher	= "3ad77bb40d7a3660a89ecaf32466ef97",
		}, {
			.iv	= NULL,
			.key	= "2b7e151628aed2a6abf7158809cf4f3c",
			.plain	= "ae2d8a571e03ac9c9eb76fac45af8e51",
			.cipher	= "f5d3d58503b9699de785895a96fdbaaf",
		}, {
			.iv	= NULL,
			.key	= "2b7e151628aed2a6abf7158809cf4f3c",
			.plain	= "30c81c46a35ce411e5fbc1191a0a52ef",
			.cipher	= "43b1cd7f598ece23881b00e3ed030688",
		}, {
			.iv	= NULL,
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
			.iv	= NULL,
			.key	= "8e73b0f7da0e6452c810f32b809079e562f8ead2522c6b7b",
			.plain	= "6bc1bee22e409f96e93d7e117393172a",
			.cipher	= "bd334f1d6e45f25ff712a214571fa5cc",
		}, {
			.iv	= NULL,
			.key	= "8e73b0f7da0e6452c810f32b809079e562f8ead2522c6b7b",
			.plain	= "ae2d8a571e03ac9c9eb76fac45af8e51",
			.cipher	= "974104846d0ad3ad7734ecb3ecee4eef",
		}, {
			.iv	= NULL,
			.key	= "8e73b0f7da0e6452c810f32b809079e562f8ead2522c6b7b",
			.plain	= "30c81c46a35ce411e5fbc1191a0a52ef",
			.cipher	= "ef7afd2270e2e60adce0ba2face6444e",
		}, {
			.iv	= NULL,
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

int main(int argc, char *argv[])
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(check_cbc128),
		cmocka_unit_test(check_cbc192),
		cmocka_unit_test(check_cbc256),
	};

	unlink(__FILE__".xml");
	setenv("CMOCKA_XML_FILE", __FILE__".xml", 1);
	cmocka_set_message_output(CM_OUTPUT_XML);
	return cmocka_run_group_tests_name("cipher", tests, NULL, NULL);
}
