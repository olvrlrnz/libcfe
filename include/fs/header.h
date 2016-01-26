#ifndef LIBCFE_FS_HEADER_H
#define LIBCFE_FS_HEADER_H


#include <inttypes.h>
#include <pthread.h>
#include <cfe/atomic.h>
#include <cfe/list.h>


#define CFE_HEADER_VERSION_DEFAULT		-1


struct cfe_header_type;


struct cfe_header {
	pthread_rwlock_t lock;
	struct cfe_header_type *type;
	const struct cfe_header_ops *ops;
};

struct cfe_header_ops {
	void (*free)(struct cfe_header *header);
	struct cfe_header *(*init)(void);
};

struct cfe_header_type {
	uint16_t version;
	cfe_atomic_t refcnt;
	struct list_head next;

	struct cfe_header *(*alloc)(int fd, size_t size);
};


extern int cfe_header_register(struct cfe_header_type *type);

extern int cfe_header_unregister(struct cfe_header_type *type);


extern void cfe_header_close(struct cfe_header *header);

extern struct cfe_header *cfe_header_create(int version, int fd, size_t size);


#endif
