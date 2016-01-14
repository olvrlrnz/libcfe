#include <stdlib.h>
#include <keyutils.h>
#include <cfe/compiler.h>
#include <cfe/keystore.h>
#include <cfe/log.h>

static key_serial_t ks_linux_find_key(const char *description)
{
	return request_key("user", description, NULL, KEY_SPEC_USER_KEYRING);
}

static void ks_linux_remove_key(const char *desc)
{
	key_serial_t key;

	key = ks_linux_find_key(desc);
	if (key == -1)
		return;

	if (keyctl_invalidate(key))
		cfe_log_notice("Unable to remove key '%s': %m", desc);
}

static int ks_linux_add_key(const char *desc, const void *payload, size_t len)
{
	key_serial_t key;

	key = add_key("user", desc, payload, len, KEY_SPEC_USER_KEYRING);
	return key == -1 ? 1 : 0;
}

static int ks_linux_read_key(const char *desc, void *buffer, size_t *len)
{
	long ret;
	key_serial_t key;

	key = ks_linux_find_key(desc);
	if (key == -1)
		return 1;

	ret = keyctl_read(key, buffer, *len);
	if (ret == -1) {
		ret = 1;
	} else {
		*len = ret;
		ret = 0;
	}

	return ret;
}

static const struct cfe_keystore_ops ks_linux_ops = {
	.remove_key = ks_linux_remove_key,
	.add_key = ks_linux_add_key,
	.read_key = ks_linux_read_key,
};

static const struct cfe_keystore_type ks_linux = {
	.ks_name = "keystore-linux",
	.ks_ops = &ks_linux_ops
};

static _constructor_ void ks_linux_register_type(void)
{
	if (cfe_keystore_set_default(&ks_linux)) {
		cfe_log_critical("could not set default key store");
		abort();
	}
}
