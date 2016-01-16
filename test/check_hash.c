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
#include <crypto/hash.h>


static const char *test_string = "The quick brown fox jumps over the lazy dog";


static void convert_hexbuffer(unsigned char *in, size_t isize, char *out)
{
	size_t i;
	char *tmp;

	for (i = 0, tmp = out; i < isize; ++i)
		tmp += sprintf(tmp, "%02x", in[i]);

	*tmp = '\0';
}

static void generic_check_hash(const char *algname, unsigned int mdsize,
                               const char *input, const char *exptected)
{
	int res;
	size_t isize;
	struct cfe_hash_ctx *ctx;
	unsigned char hbuf[mdsize];
	char output[2 * mdsize + 1];

	isize = strlen(input);

	ctx = cfe_hash_alloc_ctx(algname);
	assert_ptr_not_equal(ctx, NULL);

	res = cfe_hash_ctx_digestsize(ctx);
	assert_int_equal(res, mdsize);

	res = cfe_hash_ctx_init(ctx);
	assert_int_equal(res, 0);

	res = cfe_hash_ctx_update(ctx, input, isize);
	assert_int_equal(res, 0);

	res = cfe_hash_ctx_finalize(ctx, hbuf);
	assert_int_equal(res, 0);

	convert_hexbuffer(hbuf, mdsize, output);
	res = strcmp(output, exptected);
	assert_int_equal(res, 0);

	cfe_hash_destroy_ctx(ctx);
}

static void check_sha1(void **state)
{
	generic_check_hash("sha1", 20, test_string, "2fd4e1c67a2d28fced849ee1" \
                           "bb76e7391b93eb12");
}

static void check_sha224(void **state)
{
	generic_check_hash("sha224", 28, test_string, "730e109bd7a8a32b1cb9d9" \
	                   "a09aa2325d2430587ddbc0c38bad911525");
}

static void check_sha256(void **state)
{
	generic_check_hash("sha256", 32, test_string, "d7a8fbb307d7809469ca9a" \
	                   "bcb0082e4f8d5651e46d3cdb762d02d0bf37c9e592");
}

static void check_sha384(void **state)
{
	generic_check_hash("sha384", 48, test_string, "ca737f1014a48f4c0b6dd4" \
	                   "3cb177b0afd9e5169367544c494011e3317dbf9a509cb1e5d" \
	                   "c1e85a941bbee3d7f2afbc9b1");
}

static void check_sha512(void **state)
{
	generic_check_hash("sha512", 64, test_string, "07e547d9586f6a73f73fba" \
	                   "c0435ed76951218fb7d0c8d788a309d785436bbb642e93a25" \
	                   "2a954f23912547d1e8a3b5ed6e1bfd7097821233fa0538f3d" \
	                   "b854fee6");
}

int main(int argc, char *argv[])
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(check_sha1),
		cmocka_unit_test(check_sha224),
		cmocka_unit_test(check_sha256),
		cmocka_unit_test(check_sha384),
		cmocka_unit_test(check_sha512),
	};

	unlink(__FILE__".xml");
	setenv("CMOCKA_XML_FILE", __FILE__".xml", 1);
	cmocka_set_message_output(CM_OUTPUT_XML);
	return cmocka_run_group_tests_name("hash", tests, NULL, NULL);
}
