#ifndef LIBCFE_CFE_KEYSTORE_H
#define LIBCFE_CFE_KEYSTORE_H


#include <stdlib.h>


struct cfe_keystore_ops {
	void (*remove_key)(const char *description);
	int (*add_key)(const char *description, const void *payload, size_t len);
	int (*read_key)(const char *description, void *buffer, size_t *len);
};

struct cfe_keystore_type {
	const char *ks_name;
	const struct cfe_keystore_ops *ks_ops;
};


extern int cfe_keystore_set_default(const struct cfe_keystore_type *type);


extern int cfe_keystore_add_key(const char *description, const void *payload, size_t len);
extern void cfe_keystore_remove_key(const char *description);
extern int cfe_keystore_read_key(const char *description, void *buffer, size_t *len);


#endif
