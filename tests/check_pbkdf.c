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
#include <crypto/pbkdf.h>
#include "utils.h"


struct data {
	const char *password;
	const char *salt;
	unsigned rounds;
	const char *expected;
	unsigned keysize;
};


static void _generic_check_pbkdf(const char *alg, const struct data *data)
{
	int ret;

	size_t passwordlen = strlen(data->password);
	size_t saltlen = strlen(data->salt);

	unsigned char salt[128];
	unsigned char expected[128];
	unsigned char result[128];

	hex_str_to_bytes(data->salt, salt);
	hex_str_to_bytes(data->expected, expected);

	ret = cfe_pbkdf_derive(alg, data->rounds, data->password, passwordlen,
	                       salt, saltlen / 2, result, data->keysize);
	assert_int_equal(ret, 0);

	ret = memcmp(result, expected, data->keysize);
	assert_int_equal(ret, 0);
}

static void generic_check_pbkdf(const char *alg, const struct data *data, size_t num)
{
	size_t i;

	for (i = 0; i < num; ++i)
		_generic_check_pbkdf(alg, data + i);
}

static void check_pbkdf_sha1(void **state)
{
	struct data data[] = {
		{
			.password	= "password",
			.salt		= "73616c74",
			.rounds		= 1,
			.expected	= "0c60c80f961f0e71f3a9b524af601206"
			                  "2fe037a6",
			.keysize	= 20
		},
		{
			.password	= "password",
			.salt		= "73616c74",
			.rounds		= 2,
			.expected	= "ea6c014dc72d6f8ccd1ed92ace1d41f0d"
			                  "8de8957",
			.keysize	= 20
		},
		{
			.password	= "password",
			.salt		= "73616c74",
			.rounds		= 4096,
			.expected	= "4b007901b765489abead49d926f721d06"
			                  "5a429c1",
			.keysize	= 20
		},
		{
			.password	= "password",
			.salt		= "73616c74",
			.rounds		= 16777216,
			.expected	= "eefe3d61cd4da4e4e9945b3d6ba2158c"
			                  "2634e984",
			.keysize	= 20
		},
	};

	generic_check_pbkdf("pbkdf-sha1", data, ARRAY_SIZE(data));
}

static void check_pbkdf_sha256(void **state)
{
	struct data data[] = {
		{
			.password	= "password",
			.salt		= "73616c74",
			.rounds		= 1,
			.expected	= "120fb6cffcf8b32c43e7225256c4f837"
			                  "a86548c9",
			.keysize	= 20
		},
		{
			.password	= "password",
			.salt		= "73616c74",
			.rounds		= 2,
			.expected	= "ae4d0c95af6b46d32d0adff928f06dd0"
			                  "2a303f8e",
			.keysize	= 20
		},
		{
			.password	= "password",
			.salt		= "73616c74",
			.rounds		= 4096,
			.expected	= "c5e478d59288c841aa530db6845c4c8d"
			                  "962893a0",
			.keysize	= 20
		},
		{
			.password	= "password",
			.salt		= "73616c74",
			.rounds		= 16777216,
			.expected	= "cf81c66fe8cfc04d1f31ecb65dab4089"
			                  "f7f179e8",
			.keysize	= 20
		},
	};

	generic_check_pbkdf("pbkdf-sha256", data, ARRAY_SIZE(data));
}

int main(int argc, char *argv[])
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(check_pbkdf_sha1),
		cmocka_unit_test(check_pbkdf_sha256),
	};

	unlink(__FILE__".xml");
	setenv("CMOCKA_XML_FILE", __FILE__".xml", 1);
	cmocka_set_message_output(CM_OUTPUT_XML);
	return cmocka_run_group_tests_name("pbkdf", tests, NULL, NULL);
}
