#include <errno.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <cfe/keystore.h>

static void test_add(void **state)
{
	int res;
	char password[] = "AAAA";
	size_t size = sizeof(password);

	res = cfe_keystore_add_key("test1", password, size);
	assert_int_equal(res, 0);
}

static void test_read_existing(void **state)
{
	int res;
	char password[10];
	size_t size = sizeof(password);

	__builtin_memset(password, 0, size);
	res = cfe_keystore_read_key("test1", password, &size);
	assert_int_equal(res, 0);
	assert_int_equal(size, 5);

	res = __builtin_memcmp(password, "AAAA", 5);
	assert_int_equal(res, 0);
}

static void test_remove(void **state)
{
	cfe_keystore_remove_key("test1");
}

static void test_read_nonexisting(void **state)
{
	int res;
	char password[10];
	size_t size = sizeof(password);

	__builtin_memset(password, 0, size);
	res = cfe_keystore_read_key("test1", password, &size);
	assert_int_equal(res, 1);
}

int main(int argc, char *argv[])
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_add),
		cmocka_unit_test(test_read_existing),
		cmocka_unit_test(test_remove),
		cmocka_unit_test(test_read_nonexisting),
	};

	unlink(__FILE__".xml");
	setenv("CMOCKA_XML_FILE", __FILE__".xml", 1);
	cmocka_set_message_output(CM_OUTPUT_XML);
	return cmocka_run_group_tests_name("keystore", tests, NULL, NULL);
}

