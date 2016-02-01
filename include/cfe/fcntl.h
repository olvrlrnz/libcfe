#ifndef LIBCFE_CFE_FCNTL_H
#define LIBCFE_CFE_FCNTL_H


#include <sys/types.h>


extern int cfe_fallocate(int fd, off_t offset, off_t len);


#endif
