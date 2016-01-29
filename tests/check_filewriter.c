#include <errno.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdlib.h>
#include <unistd.h>
#include <cmocka.h>
#include <fs/filewriter.h>

static struct cfe_filewriter_type writer = {
	.version = 1337
};

static void register_writer(void **state)
{
	int res;

	res = cfe_filewriter_register(&writer);
	assert_int_equal(res, 0);
}

static void register_writer_again(void **state)
{
	int res;

	res = cfe_filewriter_register(&writer);
	assert_int_equal(errno, EBUSY);
	assert_int_equal(res, 1);
}

int main(int argc, char *argv[])
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(register_writer),
		cmocka_unit_test(register_writer_again),
	};

	unlink(__FILE__".xml");
	setenv("CMOCKA_XML_FILE", __FILE__".xml", 1);
	cmocka_set_message_output(CM_OUTPUT_XML);
	return cmocka_run_group_tests_name("file_writer", tests, NULL, NULL);
}
