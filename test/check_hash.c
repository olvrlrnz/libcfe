#include <errno.h>
#include <check.h>
#include <string.h>
#include <stdio.h>
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
	ck_assert_ptr_ne(ctx, NULL);

	res = cfe_hash_ctx_digestsize(ctx);
	ck_assert_int_eq(res, mdsize);

	res = cfe_hash_ctx_init(ctx);
	ck_assert_int_eq(res, 0);

	res = cfe_hash_ctx_update(ctx, input, isize);
	ck_assert_int_eq(res, 0);

	res = cfe_hash_ctx_finalize(ctx, hbuf);
	ck_assert_int_eq(res, 0);

	convert_hexbuffer(hbuf, mdsize, output);
	res = strcmp(output, exptected);
	ck_assert_int_eq(res, 0);

	cfe_hash_destroy_ctx(ctx);
}

START_TEST(check_sha1)
{
	generic_check_hash("sha1", 20, test_string, "2fd4e1c67a2d28fced849ee1" \
                           "bb76e7391b93eb12");
}
END_TEST

START_TEST(check_sha224)
{
	generic_check_hash("sha224", 28, test_string, "730e109bd7a8a32b1cb9d9" \
	                   "a09aa2325d2430587ddbc0c38bad911525");
}
END_TEST

START_TEST(check_sha256)
{
	generic_check_hash("sha256", 32, test_string, "d7a8fbb307d7809469ca9a" \
	                   "bcb0082e4f8d5651e46d3cdb762d02d0bf37c9e592");
}
END_TEST

START_TEST(check_sha384)
{
	generic_check_hash("sha384", 48, test_string, "ca737f1014a48f4c0b6dd4" \
	                   "3cb177b0afd9e5169367544c494011e3317dbf9a509cb1e5d" \
	                   "c1e85a941bbee3d7f2afbc9b1");
}
END_TEST

START_TEST(check_sha512)
{
	generic_check_hash("sha512", 64, test_string, "07e547d9586f6a73f73fba" \
	                   "c0435ed76951218fb7d0c8d788a309d785436bbb642e93a25" \
	                   "2a954f23912547d1e8a3b5ed6e1bfd7097821233fa0538f3d" \
	                   "b854fee6");
}
END_TEST

static Suite *add_suite(void)
{
	Suite *s;
	TCase *tc;

	s = suite_create("check_hash");

	tc = tcase_create("check_sha1");
	tcase_add_test(tc, check_sha1);
	suite_add_tcase(s, tc);

	tc = tcase_create("check_sha224");
	tcase_add_test(tc, check_sha224);
	suite_add_tcase(s, tc);

	tc = tcase_create("check_sha256");
	tcase_add_test(tc, check_sha256);
	suite_add_tcase(s, tc);

	tc = tcase_create("check_sha384");
	tcase_add_test(tc, check_sha384);
	suite_add_tcase(s, tc);

	tc = tcase_create("check_sha512");
	tcase_add_test(tc, check_sha512);
	suite_add_tcase(s, tc);

	return s;
}


int main(int argc, char *argv[])
{
	int failed;
	Suite *s;
	SRunner *sr;

	s = add_suite();
	sr = srunner_create(s);

	srunner_set_tap(sr, "report.xml");
	srunner_set_fork_status(sr, CK_NOFORK);
	srunner_run_all(sr, CK_VERBOSE);
	failed = srunner_ntests_failed(sr);

	srunner_free(sr);
	return failed ? 1 : 0;
}

