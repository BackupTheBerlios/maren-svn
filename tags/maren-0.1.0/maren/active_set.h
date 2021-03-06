#ifndef MAREN_ACTIVE_SET_H
#define MAREN_ACTIVE_SET_H
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

#include <stddef.h>

#include "fact_container.h"

MAREN_BEGIN_DECL

struct sMarenContext;

typedef struct sMarenActiveSet {
  struct sMarenActiveSet* p1;
  void* p2;
  size_t size;
  struct sMarenFactContainer* facts[];
} MarenActiveSet;

MarenActiveSet* maren_active_set_from_fact( struct sMarenContext* ctx,
					    struct sMarenFactContainer* fact );

MarenActiveSet*
maren_active_set_join( struct sMarenContext* ctx,
		       const MarenActiveSet* as1, const MarenActiveSet* as2 );

void maren_active_set_delete( struct sMarenContext* ctx, MarenActiveSet* as );

#define maren_active_set_get( as, idx ) \
   ((as)->facts[ idx ]->fact)

MAREN_END_DECL
#endif /* MAREN_ACTIVE_SET_H */
