#include <errno.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdlib.h>
#include <unistd.h>
#include <cmocka.h>
#include <cfe/ref.h>

#define container_of(ptr, type, member)				\
({								\
	typeof( ((type *)0)->member ) *__mptr = (ptr);		\
	(type *)( (char *)__mptr - offsetof(type,member) );	\
})

struct dummy {
	int a;
	struct cfe_ref ref;
};

static struct dummy *dummy_new(void)
{
	struct dummy *d;

	d = calloc(1, sizeof(*d));
	if (d) {
		cfe_ref_init(&d->ref);
	}

	return d;
}

static void __dummy_put(struct cfe_ref *ref)
{
	struct dummy *d;

	d = container_of(ref, struct dummy, ref);
	free(d);
}

static int dummy_put(struct dummy *d)
{
	return cfe_ref_put(&d->ref, __dummy_put);
}

static void dummy_get(struct dummy *dummy)
{
	cfe_ref_get(&dummy->ref);
}

static void test_ref1(void **state)
{
	int res;
	static struct dummy *d;

	d = dummy_new();
	assert_ptr_not_equal(d, NULL);

	res = dummy_put(d);
	assert_int_equal(res, 1);
}

static void test_ref2(void **state)
{
	int res;
	static struct dummy *d;

	d = dummy_new();
	assert_ptr_not_equal(d, NULL);

	dummy_get(d);
	res = dummy_put(d);
	assert_int_equal(res, 0);

	res = dummy_put(d);
	assert_int_equal(res, 1);
}

int main(int argc, char *argv[])
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_ref1),
		cmocka_unit_test(test_ref2),
	};

	unlink(__FILE__".xml");
	setenv("CMOCKA_XML_FILE", __FILE__".xml", 1);
	cmocka_set_message_output(CM_OUTPUT_XML);
	return cmocka_run_group_tests_name("ref", tests, NULL, NULL);
}
