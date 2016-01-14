#include <errno.h>
#include <check.h>
#include <stdlib.h>
#include <cfe/ref.h>

#define container_of(ptr, type, member)				\
({								\
	const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
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

START_TEST(test_ref1)
{
	int res;
	static struct dummy *d;

	d = dummy_new();
	ck_assert_ptr_ne(d, NULL);

	res = dummy_put(d);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_ref2)
{
	int res;
	static struct dummy *d;

	d = dummy_new();
	ck_assert_ptr_ne(d, NULL);

	dummy_get(d);
	res = dummy_put(d);
	ck_assert_int_eq(res, 0);

	res = dummy_put(d);
	ck_assert_int_eq(res, 1);
}
END_TEST

static Suite *add_suite(void)
{
	Suite *s;
	TCase *tc;

	s = suite_create("ref");

	tc = tcase_create("test_ref1");
	tcase_add_test(tc, test_ref1);
	suite_add_tcase(s, tc);

	tc = tcase_create("test_ref2");
	tcase_add_test(tc, test_ref2);
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

