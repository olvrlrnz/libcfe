#ifndef LIBCFE_FS_FILEWRITER_H
#define LIBCFE_FS_FILEWRITER_H


#include <cfe/list.h>
#include <cfe/atomic.h>


struct cfe_filewriter_type;


struct cfe_filewriter {
	pthread_rwlock_t lock;
	struct cfe_filewriter_type *type;
	const struct cfe_filewriter_ops *ops;
};

struct cfe_filewriter_ops {
	void (*free)(struct cfe_filewriter *writer);
};

struct cfe_filewriter_type {
	unsigned version;
	cfe_atomic_t refcnt;
	struct cfe_filewriter *(*alloc)(void);
	struct list_head next;
};


extern int cfe_filewriter_register(struct cfe_filewriter_type *type);
extern int cfe_filewriter_unregister(struct cfe_filewriter_type *type);

extern struct cfe_filewriter *cfe_filewriter_create(int version);

#endif
