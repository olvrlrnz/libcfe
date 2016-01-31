#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>
#include <fs/filewriter.h>
#include <cfe/compiler.h>
#include <cfe/malloc.h>
#include <crypto/cipher.h>
#include <sys/mman.h>


struct cfe_filewriter_v1 {
	struct cfe_filewriter base;
};

static struct cfe_filewriter_v1 *V1_FILEWRITER(struct cfe_filewriter *writer)
{
	return container_of(writer, struct cfe_filewriter_v1, base);
}

static void cfe_filewriter_v1_destroy(struct cfe_filewriter_v1 *writer)
{
	if (!writer)
		return;

	pthread_rwlock_destroy(&writer->base.lock);
	cfe_free_secure(writer->base.key);
}

static void cfe_filewriter_v1_free(struct cfe_filewriter_v1 *writer)
{
	cfe_filewriter_v1_destroy(writer);
	cfe_free(writer);
}

static void cfe_filewriter_free(struct cfe_filewriter *writer)
{
	if (!writer)
		return;

	cfe_filewriter_v1_free(V1_FILEWRITER(writer));
}

static int cfe_filewriter_v1_init(struct cfe_filewriter_v1 *writer)
{
	pthread_rwlock_init(&writer->base.lock, NULL);
	return 0;
}

static struct cfe_filewriter_v1 *cfe_filewriter_v1_new(void)
{
	struct cfe_filewriter_v1 *writer;

	writer = cfe_calloc(1, sizeof(*writer));
	if (!writer)
		return NULL;

	if (cfe_filewriter_v1_init(writer)) {
		cfe_filewriter_v1_free(writer);
		return NULL;
	}

	return writer;
}


static const struct cfe_filewriter_ops cfe_filewriter_v1_ops = {
	.free = cfe_filewriter_free,
};


static struct cfe_filewriter *
cfe_filewriter_v1_alloc(struct cfe_cipher_type *cipher, uint32_t blocksize)
{
	struct cfe_filewriter_v1 *writer;

	writer = cfe_filewriter_v1_new();
	if (!writer)
		return NULL;

	writer->base.ops = &cfe_filewriter_v1_ops;
	return &writer->base;
}

static struct cfe_filewriter_type writer_v1_type = {
	.version = 1,
	.alloc = cfe_filewriter_v1_alloc,
};

static _constructor_ void writer_v1_init(void)
{
	if (cfe_filewriter_register(&writer_v1_type))
		abort();
}
