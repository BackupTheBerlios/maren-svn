#ifndef MARFEN_NODE_FUNCTIONS_H
#define MARFEN_NODE_FUNCTIONS_H

#include <stdbool.h>

#include "decls.h"

MAREN_BEGIN_DECL

struct sMarenActiveSet;
struct sMarenContext;

typedef bool (*MarenSingleCheckFn)( void* data, const void* fact );

typedef bool (*MarenDoubleCheckFn)( void* data,
				    const void* f1, const void* f2 );

typedef void (*MarenActionFn)( void* data,
			       struct sMarenContext* ctx,
			       const struct sMarenActiveSet* active_set );

MAREN_END_DECL
#endif /* MARFEN_NODE_FUNCTIONS_H */
