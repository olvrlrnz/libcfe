#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <cmocka.h>
#include <cfe/cfe.h>


static void check_create(void **state)
{
	int fd;
	char name[16];
	struct cfe_file *file;
	struct cfe_open_params params = {
		.header_size = 0,
		.key_ident = "cfe:nonexisting",
	};

	memset(name, 0, sizeof(name));
	strcpy(name, "libcfe_XXXXXX");
	fd = mkstemp(name);
	assert_int_not_equal(fd, -1);

	file = cfe_create(fd, O_CREAT | O_EXCL | O_TRUNC, 0600, &params);
	assert_ptr_not_equal(file, NULL);
	assert_int_equal(params.header_size, 4096U);
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
