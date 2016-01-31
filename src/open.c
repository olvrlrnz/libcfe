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
#include <fs/filewriter.h>
#include <sys/stat.h>


struct cfe_file {
	struct cfe_header *header;
	struct cfe_filewriter *writer;
	cfe_atomic_t refcnt;
};

static void cfe_file_destroy(struct cfe_file *file)
{
	if (!file)
		return;

	if (file->header)
		cfe_header_close(file->header);
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



static struct cfe_file *do_open(int fd, int flags,
                                struct cfe_open_params *params)
{
	errno = ENOSYS;
	return NULL;
}


static struct cfe_file *do_create(int fd, int flags, mode_t mode,
                                  struct cfe_open_params *params)
{
	struct cfe_file *file = NULL;

	file = cfe_file_new();
	if (!file)
		return NULL;

	file->header = cfe_header_create(CFE_HEADER_VERSION_DEFAULT,
	                                 fd, params->header_size);
	if (!file->header) {
		goto failed;
	}

	file->writer = cfe_filewriter_create(CFE_FILEWRITER_VERSION_DEFAULT,
	                                     params->cipher, params->blocksize);
	if (!file->writer) {
		goto failed;
	}

	return file;

failed:
	cfe_file_free(file);
	return NULL;
}

struct cfe_file *cfe_open(int fd, int flags, mode_t mode,
                          struct cfe_open_params *params)
{
	errno = ENOSYS;
	return NULL;
}

struct cfe_file *cfe_create(int fd, int flags, mode_t mode,
                            struct cfe_open_params *params)
{
	struct stat st;

	if (fd < 0 || !params || !params->key_ident) {
		errno = EINVAL;
		return NULL;
	}

	if (strncmp(params->key_ident, "cfe:", 4)) {
		errno = EINVAL;
		return NULL;
	}

	params->key_ident += 4;

	memset(&st, 0, sizeof(st));
	if (fstat(fd, &st))
		return NULL;

	if (!S_ISREG(st.st_mode) || st.st_size != 0) {
		errno = EINVAL;
		return NULL;
	}

	return do_create(fd, flags, mode, params);
}
