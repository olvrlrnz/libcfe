#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <cfe/atomic.h>
#include <crypto/pbkdf.h>


static LIST_HEAD(alg_list);
static pthread_rwlock_t alg_lock = PTHREAD_RWLOCK_INITIALIZER;


static struct cfe_pbkdf_type *find_alg(const char *name)
{
	struct list_head *pos;

	list_for_each(pos, &alg_list) {
		struct cfe_pbkdf_type *tmp;

		tmp = list_entry(pos, struct cfe_pbkdf_type, next);
		if (strcmp(name, tmp->name) == 0)
			return tmp;
	}

	return NULL;
}

int cfe_pbkdf_register(struct cfe_pbkdf_type *type)
{
	int ret;
	struct cfe_pbkdf_type *tmp;

	if (!type->name || !type->derive) {
		errno = EINVAL;
		return 1;
	}

	pthread_rwlock_wrlock(&alg_lock);
	{
		tmp = find_alg(type->name);
		if (tmp) {
			errno = EBUSY;
			ret = 1;
			goto unlock;
		}

		cfe_atomic_set(&type->refcnt, 1);
		list_add_tail(&type->next, &alg_list);
		ret = 0;
	}
unlock:
	pthread_rwlock_unlock(&alg_lock);
	return ret;
}

int cfe_pbkdf_unregister(struct cfe_pbkdf_type *type)
{
	int ret;

	pthread_rwlock_wrlock(&alg_lock);
	{
		if (cfe_atomic_get(&type->refcnt) != 1) {
			errno = EBUSY;
			ret = 1;
			goto unlock;
		}

		list_del_init(&type->next);
		ret = 0;
	}
unlock:
	pthread_rwlock_unlock(&alg_lock);
	return ret;
}

int cfe_pbkdf_derive(const char *algname, unsigned rounds,
                     const void *in, size_t ilen,
                     const void *salt, size_t slen,
                     void *out, size_t olen)
{
	int ret;
	struct cfe_pbkdf_type *type;

	if (!algname || !in || !ilen) {
		errno = EINVAL;
		return 1;
	}

	pthread_rwlock_rdlock(&alg_lock);
	{
		type = find_alg(algname);
		if (type)
			cfe_atomic_inc(&type->refcnt);
	}
	pthread_rwlock_unlock(&alg_lock);

	if (!type) {
		errno = ENOSYS;
		return 1;
	}

	ret = type->derive(rounds, in, ilen, salt, slen, out, olen);
	cfe_atomic_dec(&type->refcnt);

	return ret;
}
