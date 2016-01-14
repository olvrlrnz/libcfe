#include <errno.h>
#include <cfe/log.h>
#include <cfe/keystore.h>


static const struct cfe_keystore_type *keystore;

int cfe_keystore_set_default(const struct cfe_keystore_type *type)
{
	if (!type) {
		errno = EINVAL;
		return 1;
	}

	if (keystore) {
		errno = EBUSY;
		return 1;
	}

	cfe_log_notice("Keystore type '%s' set as default", type->ks_name);
	keystore = type;
	return 0;
}

int cfe_keystore_add_key(const char *description, const void *payload, size_t len)
{
	return keystore->ks_ops->add_key(description, payload, len);
}


void cfe_keystore_remove_key(const char *description)
{
	keystore->ks_ops->remove_key(description);
}

int cfe_keystore_read_key(const char *description, void *buffer, size_t *len)
{
	return keystore->ks_ops->read_key(description, buffer, len);
}
