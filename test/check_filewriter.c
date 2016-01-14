#include <errno.h>
#include <check.h>
#include <fs/file_writer.h>

static struct cfe_filewriter_type writer = {
	.version = 1
};

START_TEST(register_writer)
{
	int res;

	res = cfe_filewriter_register(&writer);
	ck_assert_int_eq(res, 0);
}
END_TEST

START_TEST(register_writer_again)
{
	int res;

	res = cfe_filewriter_register(&writer);
	ck_assert_int_eq(errno, EBUSY);
	ck_assert_int_eq(res, 1);
}
END_TEST


static Suite *add_suite(void)
{
	Suite *s;
	TCase *tc;

	s = suite_create("register");

	tc = tcase_create("register_file_writer1");
	tcase_add_test(tc, register_writer);
	suite_add_tcase(s, tc);

	tc = tcase_create("register_file_writer2");
	tcase_add_test(tc, register_writer_again);
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

