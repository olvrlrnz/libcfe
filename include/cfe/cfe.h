#ifndef LIBCFE_CFE_CFE_H
#define LIBCFE_CFE_CFE_H


#include <inttypes.h>
#include <stdlib.h>
#include <sys/types.h>


struct cfe_file;


struct cfe_open_params {
	uint32_t header_size;
	const char *key_ident;
};


extern struct cfe_file *cfe_open(const char *pathname, int flags, mode_t mode,
                                 struct cfe_open_params *params);

#endif
