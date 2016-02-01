#ifndef LIBCFE_FS_FILEWRITER_H
#define LIBCFE_FS_FILEWRITER_H


#include <inttypes.h>
#include <pthread.h>
#include <cfe/list.h>
#include <cfe/atomic.h>


#define CFE_FILEWRITER_VERSION_DEFAULT		(-1)


struct cfe_filewriter_type;
struct cfe_cipher_type;


struct cfe_filewriter {
	uint32_t blocksize;
	pthread_rwlock_t lock;
	unsigned char *key;
	size_t keysize;
	struct cfe_cipher_type *cipher;
	struct cfe_filewriter_type *type;
	const struct cfe_filewriter_ops *ops;
};

struct cfe_filewriter_ops {
	void (*free)(struct cfe_filewriter *writer);
};

struct cfe_filewriter_type {
	unsigned version;
	cfe_atomic_t refcnt;
	struct cfe_filewriter *(*alloc)(struct cfe_cipher_type *cipher,
	                                uint32_t blocksize);
	struct list_head next;
};


extern int cfe_filewriter_register(struct cfe_filewriter_type *type);
extern int cfe_filewriter_unregister(struct cfe_filewriter_type *type);

extern struct cfe_filewriter *cfe_filewriter_create(int version,
                                                    const char *cipher,
                                                    uint32_t blocksize);

#endif
