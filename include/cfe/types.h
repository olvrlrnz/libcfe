#ifndef LIBCFE_TYPES_H
#define LIBCFE_TYPES_H


#include <cfe/compiler.h>


static inline int safe_close(int fd)
{
	if (fd >= 0) {
		int old_errno = errno;
		close(fd);
		errno = old_errno;
	}

	return -1;
}

static inline void closep(int *fd)
{
	safe_close(*fd);
}

static inline void freep(void **p)
{
	free(*(void**) p);
}


#define _cleanup_close_			_cleanup_(closep)
#define _cleanup_free_			_cleanup_(freep)
#define autofd				_cleanup_close_ int



#endif
