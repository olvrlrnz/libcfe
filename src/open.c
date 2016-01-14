#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <cfe/atomic.h>
#include <cfe/cfe.h>
#include <cfe/hash.h>
#include <cfe/utils.h>
#include <cfe/macros.h>
#include <cfe/types.h>
#include <fs/header.h>
#include <fs/file_writer.h>
#include <sys/stat.h>
#include "fs/header-internal.h"


struct cfe_file {
	struct cfe_header *header;
	cfe_atomic_t refcnt;
};

static void cfe_file_destroy(struct cfe_file *file)
{
	if (!file)
		return;

	if (file->header)
		cfe_header_free(file->header);
}

static void cfe_file_free(struct cfe_file *file)
{
	cfe_file_destroy(file);
	free(file);
}

static int cfe_file_init(struct cfe_file *file)
{
	cfe_atomic_set(&file->refcnt, 1);
	return 0;
}

static struct cfe_file *cfe_file_new(void)
{
	struct cfe_file *file;

	file = calloc(1, sizeof(*file));
	if (file) {
		if (cfe_file_init(file)) {
			cfe_file_free(file);
			return NULL;
		}
	}

	return file;
}



static struct cfe_file *do_open(const char *pathname, int flags,
                                struct cfe_open_params *params)
{
	errno = ENOSYS;
	return NULL;
}


static struct cfe_file *do_create(const char *pathname, int flags, mode_t mode,
                                  struct cfe_open_params *params)
{
	autofd fd = -1;
	struct cfe_file *file = NULL;

	file = cfe_file_new();
	if (!file)
		return NULL;

	fd = open(pathname, flags, mode);
	if (fd < 0) {
		goto failed;
	}

	params->header_size = normalize_headersize(params->header_size);
	file->header = cfe_header_create(CFE_HEADER_VERSION_DEFAULT,
	                                 fd, params->header_size);
	if (!file->header) {
		goto failed;
	}

failed:
	cfe_file_free(file);
	unlink(pathname);
	return NULL;
}

struct cfe_file *cfe_open(const char *pathname, int flags, mode_t mode,
                          struct cfe_open_params *params)
{
	int ret;
	struct stat st;

	if (!pathname || !params) {
		errno = EINVAL;
		return NULL;
	}

	if (!params->key_ident && ((flags & O_CREAT) || (flags & O_TRUNC))) {
		errno = EINVAL;
		return NULL;
	}

	if (params->key_ident) {
		if (strncmp(params->key_ident, "cfe:", 4)) {
			errno = EKEYREJECTED;
			return NULL;
		}
		params->key_ident += 4;
	}

	ret = stat(pathname, &st);
	if (ret == 0) {
		if (S_ISDIR(st.st_mode)) {
			errno = EISDIR;
			return NULL;
		} else if (!S_ISREG(st.st_mode)) {
			errno = EPERM;
			return NULL;
		}

		return do_open(pathname, flags, params);
	}

	if (errno != ENOENT)
		return NULL;

	return do_create(pathname, flags, mode, params);
}
