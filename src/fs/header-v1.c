#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <fs/header.h>
#include <cfe/compiler.h>
#include <cfe/malloc.h>
#include <sys/mman.h>


struct cfe_header_v1 {
	struct cfe_header base;
	void *mapping;
	size_t msize;
};

static struct cfe_header_v1 *V1_HEADER(struct cfe_header *header)
{
	return container_of(header, struct cfe_header_v1, base);
}

static void cfe_header_v1_destroy(struct cfe_header_v1 *header)
{
	if (!header)
		return;

	if (header->mapping)
		munmap(header->mapping, header->msize);

	pthread_rwlock_destroy(&header->base.lock);
}

static void cfe_header_v1_free(struct cfe_header_v1 *header)
{
	cfe_header_v1_destroy(header);
	cfe_free(header);
}

static void cfe_header_free(struct cfe_header *header)
{
	if (!header)
		return;

	cfe_header_v1_free(V1_HEADER(header));
}

static int cfe_header_v1_init(struct cfe_header_v1 *header)
{
	pthread_rwlock_init(&header->base.lock, NULL);
	return 0;
}

static struct cfe_header_v1 *cfe_header_v1_new(void)
{
	struct cfe_header_v1 *header;

	header = cfe_calloc(1, sizeof(*header));
	if (!header)
		return NULL;

	if (cfe_header_v1_init(header)) {
		cfe_header_v1_free(header);
		return NULL;
	}

	return header;
}


static const struct cfe_header_ops cfe_header_v1_ops = {
	.free = cfe_header_free,
	.init = NULL,
};


static struct cfe_header *cfe_header_v1_alloc(int fd, size_t size)
{
	int ret;
	struct cfe_header_v1 *header;

	header = cfe_header_v1_new();
	if (!header)
		return NULL;

	ret = posix_fallocate(fd, 0, size);
	if (ret) {
		errno = ret;
		goto failed;
	}

	header->base.ops = &cfe_header_v1_ops;
	header->msize = size;
	header->mapping = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (header->mapping == MAP_FAILED)
		goto failed;

	return &header->base;

failed:
	cfe_header_v1_free(header);
	return NULL;
}

static struct cfe_header_type header_v1_type = {
	.version = 1,
	.alloc = cfe_header_v1_alloc,
};

static _constructor_ void header_v1_init(void)
{
	if (cfe_header_register(&header_v1_type))
		abort();
}
