#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <cfe/atomic.h>
#include <crypto/cipher.h>


static LIST_HEAD(alg_list);
static pthread_rwlock_t alg_lock = PTHREAD_RWLOCK_INITIALIZER;


static struct cfe_cipher_type *find_alg(const char *name)
{
	struct list_head *pos;

	list_for_each(pos, &alg_list) {
		struct cfe_cipher_type *tmp;

		tmp = list_entry(pos, struct cfe_cipher_type, next);
		if (strcmp(name, tmp->name) == 0)
			return tmp;
	}

	return NULL;
}

int cfe_cipher_register(struct cfe_cipher_type *type)
{
	int ret;
	struct cfe_cipher_type *tmp;

	if (!type->name || !type->blocksize || !type->alloc_ctx) {
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

int cfe_cipher_unregister(struct cfe_cipher_type *type)
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

struct cfe_cipher_ctx *cfe_cipher_alloc_ctx(const char *algname)
{
	struct cfe_cipher_ctx *ctx;
	struct cfe_cipher_type *type;

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

void cfe_cipher_destroy_ctx(struct cfe_cipher_ctx *ctx)
{
	struct cfe_cipher_type *type;

	if (!ctx)
		return;

	type = ctx->type;
	ctx->ops->destroy(ctx);
	cfe_atomic_dec(&type->refcnt);
}


int cfe_cipher_ctx_init(struct cfe_cipher_ctx *ctx, enum cfe_cipher_mode mode,
                        const unsigned char *key, size_t ksize,
                        const unsigned char *iv, size_t isize)
{
	struct cfe_cipher_type *type;

	if (!ctx) {
		errno = EINVAL;
		return 1;
	}

	type = ctx->type;
	if (iv && isize != type->ivsize) {
		/* iv is optional */
		errno = EINVAL;
		return 1;
	}

	if (ksize != type->keysize) {
		/* key is mandatory */
		errno = EINVAL;
		return 1;
	}

	return ctx->ops->init(ctx, mode, key, iv);
}

int cfe_cipher_ctx_update(struct cfe_cipher_ctx *ctx,
                          unsigned char *in, size_t ilen,
                          unsigned char *out, size_t *olen)
{
	if (!ctx) {
		errno = EINVAL;
		return 1;
	}

	return ctx->ops->update(ctx, in, ilen, out, olen);
}

int cfe_cipher_ctx_finalize(struct cfe_cipher_ctx *ctx,
                            unsigned char *out, size_t *olen)
{
	if (!ctx) {
		errno = EINVAL;
		return 1;
	}

	return ctx->ops->finalize(ctx, out, olen);
}

int cfe_cipher_ctx_blocksize(struct cfe_cipher_ctx *ctx)
{
	if (!ctx) {
		errno = EINVAL;
		return -1;
	}

	return ctx->type->blocksize;
}

int cfe_cipher_ctx_set_padding(struct cfe_cipher_ctx *ctx, int on)
{
	if (!ctx) {
		errno = EINVAL;
		return 1;
	}

	return ctx->ops->set_padding(ctx, on);
}
