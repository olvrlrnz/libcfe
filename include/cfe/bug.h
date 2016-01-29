#ifndef LIBCFE_CFE_BUG_H
#define LIBCFE_CFE_BUG_H


#include <cfe/compiler.h>


#define BUILD_BUG_ON_ZERO(e) (sizeof(struct { int:-!!(e); }))


#endif
