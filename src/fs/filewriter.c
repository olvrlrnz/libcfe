#include <errno.h>
#include <assert.h>
#include <pthread.h>
#include <string.h>
#include <fs/filewriter.h>
#include <cfe/log.h>


static LIST_HEAD(writer_list);
static struct cfe_filewriter_type *writer_default;
static pthread_rwlock_t writer_lock = PTHREAD_RWLOCK_INITIALIZER;


static struct cfe_filewriter_type *cfe_filewriter_find(unsigned int version)
{
	struct list_head *pos;

	list_for_each(pos, &writer_list) {
		struct cfe_filewriter_type *tmp;

		tmp = list_entry(pos, struct cfe_filewriter_type, next);
		if (tmp->version == version)
			return tmp;
	}

	return NULL;
}

int cfe_filewriter_register(struct cfe_filewriter_type *type)
{
	int ret;
	struct cfe_filewriter_type *tmp;

	pthread_rwlock_wrlock(&writer_lock);
	{
		tmp = cfe_filewriter_find(type->version);
		if (tmp) {
			errno = EBUSY;
			ret = 1;
			goto unlock;
		}

		cfe_atomic_set(&type->refcnt, 1);
		list_add_tail(&type->next, &writer_list);
		if (!writer_default || writer_default->version < type->version) {
			writer_default = type;
		}
		ret = 0;
	}
unlock:
	pthread_rwlock_unlock(&writer_lock);
	return ret;
}

int cfe_filewriter_unregister(struct cfe_filewriter_type *type)
{
	int ret;

	pthread_rwlock_wrlock(&writer_lock);
	{
		if (writer_default == type) {
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
	pthread_rwlock_unlock(&writer_lock);
	return ret;
}

struct cfe_filewriter *cfe_filewriter_create(int version)
{
	struct cfe_filewriter *writer;
	struct cfe_filewriter_type *type;

	pthread_rwlock_rdlock(&writer_lock);
	{
		if (version == -1)
			type = writer_default;
		else
			type = cfe_filewriter_find(version);

		if (type)
			cfe_atomic_inc(&type->refcnt);
	}
	pthread_rwlock_unlock(&writer_lock);

	if (!type) {
		errno = ENOSYS;
		return NULL;
	}

	writer = type->alloc();
	if (!writer)
		cfe_atomic_dec(&type->refcnt);

	writer->type = type;
	return writer;
}
