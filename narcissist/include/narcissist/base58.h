#pragma once

#include <sys/types.h>

#ifdef __cplusplus
namespace Narcissist {
extern "C" {
#endif

int base58enc(char *b58, size_t *b58sz, const void *data, size_t binsz);

#ifdef __cplusplus
}
}
#endif
