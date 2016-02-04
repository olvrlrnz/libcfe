#include <cfe/compiler.h>
#include <cfe/malloc.h>
#include <cfe/log.h>
#include <crypto/rand.h>
#include <openssl/engine.h>
#include <openssl/rand.h>


static int openssl_rand_get(void *buffer, size_t size)
{
	return RAND_bytes(buffer, size) != 1;
}

static struct cfe_rand_type openssl_type_rand = {
	.name		= "rand",
	.get		= openssl_rand_get,
};

static int rand_openssl_init_rdrand(void)
{
	ENGINE *e;

#if 0
	OPENSSL_cpuid_setup();
#endif
	ENGINE_load_rdrand();

	e = ENGINE_by_id("rdrand");
	if (!e)
		return 1;

	if (ENGINE_init(e) == 0)
		return 1;

	if (ENGINE_set_default(e, ENGINE_METHOD_RAND) == 0)
		return 1;

	return 0;
}

static void _constructor_ rand_openssl_init(void)
{
	int ret;

	ret = rand_openssl_init_rdrand();
	if (ret) {
		cfe_log_notice("No hardware random number generator "
		               "available. Using default");
	}

	ret = cfe_rand_register(&openssl_type_rand);
	if (ret)
		abort();
}
