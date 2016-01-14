#include <stdlib.h>
#include <fs/header.h>
#include <cfe/compiler.h>


struct cfe_header_v1 {
	struct cfe_header base;
};

static struct cfe_header_v1 *V1_HEADER(struct cfe_header *header)
{
	return container_of(header, struct cfe_header_v1, base);
}

static void cfe_header_v1_destroy(struct cfe_header_v1 *header)
{
	if (!header)
		return;
}

static void cfe_header_v1_free(struct cfe_header_v1 *header)
{
	cfe_header_v1_destroy(header);
	free(header);
}

static int cfe_header_v1_init(struct cfe_header_v1 *header)
{
	return 0;
}

static struct cfe_header_v1 *cfe_header_v1_new(void)
{
	struct cfe_header_v1 *header;

	header = calloc(1, sizeof(*header));
	if (!header)
		return NULL;

	if (cfe_header_v1_init(header)) {
		cfe_header_v1_free(header);
		return NULL;
	}

	return header;
}

static struct cfe_header *cfe_header_v1_alloc(int fd, size_t size)
{
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
