#ifndef LIBCFE_FS_FILE_WRITER_H
#define LIBCFE_FS_FILE_WRITER_H


#include <cfe/list.h>
#include <cfe/atomic.h>


struct cfe_filewriter;

struct cfe_filewriter_type {
	unsigned version;
	cfe_atomic_t refcnt;
	struct cfe_filewriter *(*allocate_writer)(void);
	struct list_head next;
};


extern int cfe_filewriter_register(struct cfe_filewriter_type *type);
extern int cfe_filewriter_unregister(struct cfe_filewriter_type *type);

extern struct cfe_filewriter *cfe_filewriter_create(int version);

#endif
