#include <pthread.h>
#include <stdlib.h>
#include <cfe/compiler.h>
#include <cfe/keystore.h>
#include <cfe/log.h>
#include <CoreFoundation/CoreFoundation.h>

static pthread_rwlock_t store_lock = PTHREAD_RWLOCK_INITIALIZER;
static CFMutableDictionaryRef store;

static void ks_darwin_remove_key(const char *desc)
{
	CFStringRef _desc = NULL;

	if (!desc) {
		cfe_log_notice("%s(): empty description", __func__);
		return;
	}

	_desc = CFStringCreateWithCString(NULL, desc, kCFStringEncodingUTF8);
	if (!_desc) {
		cfe_log_warning("%s(): CFStringCreateWithCString failed. "
		                "Key '%s' will NOT be deleted", __func__, desc);
	}

	pthread_rwlock_wrlock(&store_lock);
	{
		CFDictionaryRemoveValue(store, _desc);
	}
	pthread_rwlock_unlock(&store_lock);

	CFRelease(_desc);
}

static int ks_darwin_add_key(const char *desc, const void *payload, size_t len)
{
	CFStringRef _desc = NULL;
	CFDataRef _data = NULL;

	if (!desc || !payload || !len) {
		errno = EINVAL;
		return 1;
	}

	_desc = CFStringCreateWithCString(NULL, desc, kCFStringEncodingUTF8);
	if (!_desc) {
		errno = ENOMEM;
		return 1;
	}

	_data = CFDataCreate(NULL, payload, len);
	if (!_data) {
		CFRelease(_desc);
		errno = ENOMEM;
		return 1;
	}

	pthread_rwlock_wrlock(&store_lock);
	{
		CFDictionaryRemoveValue(store, _desc);
		CFDictionaryAddValue(store, _desc, _data);
	}
	pthread_rwlock_unlock(&store_lock);

	CFRelease(_desc);
	CFRelease(_data);
	return 0;
}

static int ks_darwin_read_key(const char *desc, void *buffer, size_t *len)
{
	size_t _len;
	CFStringRef _desc = NULL;
	CFDataRef _data = NULL;

	if (!desc || !len) {
		errno = EINVAL;
		return 1;
	}

	_desc = CFStringCreateWithCString(NULL, desc, kCFStringEncodingUTF8);
	if (!_desc) {
		errno = ENOMEM;
		return 1;
	}

	pthread_rwlock_rdlock(&store_lock);
	{
		_data = CFDictionaryGetValue(store, _desc);
		if (_data)
			CFRetain(_data);
	}
	pthread_rwlock_unlock(&store_lock);

	CFRelease(_desc);

	if (!_data) {
		errno = ENOENT;
		return 1;
	}

	_len = CFDataGetLength(_data);
	if (*len < _len) {
		CFRelease(_data);
		errno = ENOSPC;
		return 1;
	}

	*len = _len;
	if (buffer)
		CFDataGetBytes(_data, CFRangeMake(0, _len), buffer);
	CFRelease(_data);
	return 0;
}

static const struct cfe_keystore_ops ks_darwin_ops = {
	.remove_key = ks_darwin_remove_key,
	.add_key    = ks_darwin_add_key,
	.read_key   = ks_darwin_read_key,
};

static const struct cfe_keystore_type ks_darwin = {
	.ks_name    = "keystore-darwin",
	.ks_ops	    = &ks_darwin_ops
};

static _constructor_ void ks_darwin_register_type(void)
{
	store = CFDictionaryCreateMutable(NULL, 0,
	                                  &kCFTypeDictionaryKeyCallBacks,
	                                  &kCFTypeDictionaryValueCallBacks);
	if (!store) {
		cfe_log_critical("could not allocate dictionary");
		abort();
	}

	if (cfe_keystore_set_default(&ks_darwin)) {
		cfe_log_critical("could not set default key store");
		abort();
	}
}
