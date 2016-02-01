#ifndef LIBCFE_CRYPTO_PBKDF_H
#define LIBCFE_CRYPTO_PBKDF_H


#include <cfe/list.h>
#include <cfe/atomic.h>


#define CFE_PBKDF_NAMELEN		(16U)


struct cfe_pbkdf_type {
	char name[CFE_PBKDF_NAMELEN];
	cfe_atomic_t refcnt;
	struct list_head next;

	int (*derive)(unsigned rounds, const void *in, size_t ilen,
	              const void *salt, size_t slen, void *out, size_t olen);
};


extern int cfe_pbkdf_register(struct cfe_pbkdf_type *type);

extern int cfe_pbkdf_unregister(struct cfe_pbkdf_type *type);

extern int cfe_pbkdf_derive(const char *algname, unsigned rounds,
                            const void *in, size_t ilen, const void *salt,
                            size_t slen, void *out, size_t olen);

#endif
