#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <cfe/atomic.h>
#include <crypto/hash.h>


static LIST_HEAD(alg_list);
static pthread_rwlock_t alg_lock = PTHREAD_RWLOCK_INITIALIZER;


static struct cfe_hash_type *find_alg(const char *name)
{
	struct list_head *pos;

	list_for_each(pos, &alg_list) {
		struct cfe_hash_type *tmp;

		tmp = list_entry(pos, struct cfe_hash_type, next);
		if (strcmp(name, tmp->name) == 0)
			return tmp;
	}

	return NULL;
}

int cfe_hash_register(struct cfe_hash_type *type)
{
	int ret;
	struct cfe_hash_type *tmp;

	if (!type->name || !type->digestsize || !type->alloc_ctx) {
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

int cfe_hash_unregister(struct cfe_hash_type *type)
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

struct cfe_hash_ctx *cfe_hash_alloc_ctx(const char *algname)
{
	struct cfe_hash_ctx *ctx;
	struct cfe_hash_type *type;

	pthread_rwlock_rdlock(&alg_lock);
	{
		type = find_alg(algname);
		if (type)
			cfe_atomic_inc(&type->refcnt);
	}
	pthread_rwlock_unlock(&alg_lock);

	if (!type) {
		errno = ENOSYS;
		return NULL;
	}

	ctx = type->alloc_ctx();
	if (!ctx) {
		cfe_atomic_dec(&type->refcnt);
		return NULL;
	}

	ctx->type = type;
	return ctx;
}

void cfe_hash_destroy_ctx(struct cfe_hash_ctx *ctx)
{
	struct cfe_hash_type *type;

	if (!ctx)
		return;

	type = ctx->type;
	ctx->ops->destroy(ctx);
	cfe_atomic_dec(&type->refcnt);
}


int cfe_hash_ctx_init(struct cfe_hash_ctx *ctx)
{
	if (!ctx) {
		errno = EINVAL;
		return 1;
	}

	return ctx->ops->init(ctx);
}

int cfe_hash_ctx_update(struct cfe_hash_ctx *ctx, const void *buffer, size_t size)
{
	if (!ctx) {
		errno = EINVAL;
		return 1;
	}

	return ctx->ops->update(ctx, buffer, size);
}

int cfe_hash_ctx_finalize(struct cfe_hash_ctx *ctx, void *buffer)
{
	if (!ctx) {
		errno = EINVAL;
		return 1;
	}

	return ctx->ops->finalize(ctx, buffer);
}

int cfe_hash_ctx_digestsize(struct cfe_hash_ctx *ctx)
{
	if (!ctx) {
		errno = EINVAL;
		return -1;
	}

	return ctx->type->digestsize;
}
