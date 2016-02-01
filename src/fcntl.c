#include <fcntl.h>
#include <config.h>
#include <cfe/fcntl.h>

int cfe_fallocate(int fd, off_t offset, off_t len)
{
#ifdef HAVE_POSIX_FALLOCATE
	return posix_fallocate(fd, offset, len);
#endif

	fstore_t fst;
	fst.fst_flags = F_ALLOCATECONTIG;
	fst.fst_posmode = F_PEOFPOSMODE;
	fst.fst_offset = offset;
	fst.fst_length = length;
	fst.fst_bytesalloc = 0;
	return fcntl(fd, F_PREALLOCATE, &fst);
}
