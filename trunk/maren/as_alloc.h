#ifndef MAREN_AS_ALLOC_H
#define MAREN_AS_ALLOC_H
/* MAReN - Maren Aint a Rete Network
 * Copyright (C) 2004  Marcus Perlick
 * mailto: riffraff@users.berlios.de
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 *
 * This file is part of the software package MAReN */

#include <config.h>
#include <stdlib.h>

#include "active_set.h"

MAREN_BEGIN_DECL
#if MAREN_ACTIVE_SET_ALLOC != 0

# if MAREN_ACTIVE_SET_ALLOC == 1
typedef struct {
  size_t size;
  size_t count;
  void* mem;
} MarenActiveSetAllocSlot;

typedef struct sMarenActiveSetAlloc {
  /** \internal If you wanna optimize allocation for the fc-sizes
   *  1,..,3 \c as_size_end will be 4. */
  size_t as_size_end;
  size_t slot_max;
  MarenActiveSetAllocSlot* slots;
} MarenActiveSetAlloc;
# else // MAREN_ACTIVE_SET_ALLOC == 1
#  error Unkonwn allocator for active sets selected.
# endif // MAREN_ACTIVE_SET_ALLOC == 1

#ifndef MAREN_ACTIVE_SET_ALLOC_SIZE_MAX
# define MAREN_ACTIVE_SET_ALLOC_SIZE_MAX 8
#endif

#ifndef MAREN_ACTIVE_SET_ALLOC_SLOT_MAX
# define MAREN_ACTIVE_SET_ALLOC_SLOT_MAX 1024
#endif

MarenActiveSetAlloc* maren_active_set_alloc_ctor( MarenActiveSetAlloc* where,
						  size_t as_size_max,
						  size_t max_sets_per_slot );

void maren_active_set_alloc_dtor( MarenActiveSetAlloc* alloc );

void* maren_active_set_alloc_alloc( MarenActiveSetAlloc* alloc, size_t sz );
void maren_active_set_alloc_free( MarenActiveSetAlloc* alloc, 
				  struct sMarenActiveSet* as );

# define maren_alloc_as( ctx, sz ) \
   maren_active_set_alloc_alloc( &((ctx)->as_alloc), sz )

# define maren_free_as( ctx, as ) \
   maren_active_set_alloc_free( &((ctx)->as_alloc), as )

#else // MAREN_ACTIVE_SET_ALLOC != 0
# define maren_alloc_as( ctx, sz ) \
   malloc( sizeof(MarenActiveSet) + (sz) * sizeof(void*) )

# define maren_free_as( ctx, as ) \
   free( as )
#endif // MAREN_ACTIVE_SET_ALLOC != 0

MAREN_END_DECL
#endif /* MAREN_AS_ALLOC_H */
