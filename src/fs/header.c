#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <cfe/atomic.h>
#include <cfe/log.h>
#include <fs/header.h>
#include "header-internal.h"


static LIST_HEAD(header_list);
static struct cfe_header_type *header_default;
static pthread_rwlock_t header_lock = PTHREAD_RWLOCK_INITIALIZER;


static struct cfe_header_type *cfe_header_find(const uint16_t version)
{
	struct list_head *pos;

	list_for_each(pos, &header_list) {
		struct cfe_header_type *tmp;

		tmp = list_entry(pos, struct cfe_header_type, next);
		if (tmp->version == version)
			return tmp;
	}

	return NULL;
}

int cfe_header_register(struct cfe_header_type *type)
{
	int ret;
	struct cfe_header_type *tmp;

	pthread_rwlock_wrlock(&header_lock);
	{
		tmp = cfe_header_find(type->version);
		if (tmp) {
			errno = EBUSY;
			ret = 1;
			goto unlock;
		}

		cfe_atomic_set(&type->refcnt, 1);
		list_add_tail(&type->next, &header_list);
		if (!header_default || header_default->version < type->version) {
			header_default = type;
		}
		ret = 0;
	}
unlock:
	pthread_rwlock_unlock(&header_lock);
	return ret;
}

int cfe_header_unregister(struct cfe_header_type *type)
{
	int ret;

	pthread_rwlock_wrlock(&header_lock);
	{
		if (header_default == type) {
			errno = EBUSY;
			ret = 1;
			goto unlock;
		}

		if (cfe_atomic_get(&type->refcnt) != 1) {
			errno = EBUSY;
			ret = 1;
			goto unlock;
		}

		list_del_init(&type->next);
		ret = 0;
	}
unlock:
	pthread_rwlock_unlock(&header_lock);
	return ret;
}

struct cfe_header *cfe_header_create(int version, int fd, size_t size)
{
	struct cfe_header *header;
	struct cfe_header_type *type;

	pthread_rwlock_rdlock(&header_lock);
	{
		if (version == CFE_HEADER_VERSION_DEFAULT)
			type = header_default;
		else
			type = cfe_header_find(version);

		if (type)
			cfe_atomic_inc(&type->refcnt);
	}
	pthread_rwlock_unlock(&header_lock);

	if (!type) {
		errno = ENOSYS;
		return NULL;
	}

	size = normalize_headersize(size);
	header = type->alloc(fd, size);
	if (!header) {
		cfe_atomic_dec(&type->refcnt);
		return NULL;
	}

	header->type = type;
	return header;
}

void cfe_header_close(struct cfe_header *header)
{
	struct cfe_header_type *type;

	if (!header)
		return;

	type = header->type;
	header->ops->free(header);
	cfe_atomic_dec(&type->refcnt);
}
