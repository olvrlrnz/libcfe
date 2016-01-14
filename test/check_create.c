#include <check.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <cfe/cfe.h>


#if 0
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
#endif

START_TEST(check_create)
{
	struct cfe_file *file;
	struct cfe_open_params params = {
		.header_size = 123,
		.key_ident = "cfe:nonexisting",
	};

	file = cfe_open("/tmp/foobar.txt", O_CREAT | O_EXCL | O_TRUNC, 0600, &params);
	ck_assert_ptr_ne(file, NULL);
}
END_TEST

static Suite *add_suite(void)
{
	Suite *s;
	TCase *tc;

	s = suite_create("check_create");

	tc = tcase_create("check_create");
	tcase_add_test(tc, check_create);
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

	srunner_set_tap(sr, __FILE__"_report.xml");
	srunner_set_fork_status(sr, CK_NOFORK);
	srunner_run_all(sr, CK_VERBOSE);
	failed = srunner_ntests_failed(sr);

	srunner_free(sr);
	return failed ? 1 : 0;
}

