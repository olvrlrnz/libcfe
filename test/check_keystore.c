#include <errno.h>
#include <check.h>
#include <stdlib.h>
#include <cfe/keystore.h>

START_TEST(test_add)
{
	int res;
	char password[] = "AAAA";
	size_t size = sizeof(password);

	res = cfe_keystore_add_key("test1", password, size);
	ck_assert_int_eq(res, 0);
}
END_TEST

START_TEST(test_read_existing)
{
	int res;
	char password[10];
	size_t size = sizeof(password);

	__builtin_memset(password, 0, size);
	res = cfe_keystore_read_key("test1", password, &size);
	ck_assert_int_eq(res, 0);
	ck_assert_int_eq(size, 5);

	res = memcmp(password, "AAAA", 5);
	ck_assert_int_eq(res, 0);
}
END_TEST

START_TEST(test_remove)
{
	cfe_keystore_remove_key("test1");
}
END_TEST

START_TEST(test_read_nonexisting)
{
	int res;
	char password[10];
	size_t size = sizeof(password);

	__builtin_memset(password, 0, size);
	res = cfe_keystore_read_key("test1", password, &size);
	ck_assert_int_eq(res, 1);
}
END_TEST

static Suite *add_suite(void)
{
	Suite *s;
	TCase *tc;

	s = suite_create("keystore");

	tc = tcase_create("test_add");
	tcase_add_test(tc, test_add);
	suite_add_tcase(s, tc);

	tc = tcase_create("test_read_existing");
	tcase_add_test(tc, test_read_existing);
	suite_add_tcase(s, tc);

	tc = tcase_create("test_remove");
	tcase_add_test(tc, test_remove);
	suite_add_tcase(s, tc);

	tc = tcase_create("test_read_nonexisting");
	tcase_add_test(tc, test_read_nonexisting);
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

	srunner_set_tap(sr, "report_keystore.xml");
	srunner_set_fork_status(sr, CK_NOFORK);
	srunner_run_all(sr, CK_VERBOSE);
	failed = srunner_ntests_failed(sr);

	srunner_free(sr);
	return failed ? 1 : 0;
}

