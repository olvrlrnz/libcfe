#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <cfe/cfe.h>


static void check_create(void **state)
{
	struct cfe_file *file;
	struct cfe_open_params params = {
		.header_size = 123,
		.key_ident = "cfe:nonexisting",
	};

	file = cfe_open("/tmp/foobar.txt", O_CREAT | O_EXCL | O_TRUNC, 0600, &params);
	/* FIXME */
	assert_ptr_equal(file, NULL);
}

int main(int argc, char *argv[])
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(check_create),
	};

	unlink(__FILE__".xml");
	setenv("CMOCKA_XML_FILE", __FILE__".xml", 1);
	cmocka_set_message_output(CM_OUTPUT_XML);
	return cmocka_run_group_tests_name("create", tests, NULL, NULL);
}
