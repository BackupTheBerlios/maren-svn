#ifndef MAREN_FACT_CONTAINER_H
#define MAREN_FACT_CONTAINER_H
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

#include <stdbool.h>

#include "decls.h"

MAREN_BEGIN_DECL

typedef struct sMarenFactContainer {
  unsigned int refc;
  const void* fact;
  void (*del_fact)(void*);
  const void* hash_key;
  void (*del_hash_key)(void*);
  bool removed;
} MarenFactContainer;

MarenFactContainer* maren_fact_container_ctor( const void* fact,
					       void (*del_fact)(void*) );

/** \internal FactContainers are always allocated on the heap. It
 *  seems not very reasonable, to call only the dtor! */
void maren_fact_container_dtor( MarenFactContainer* factc );
#define maren_fact_container_del( factc ) \
   maren_fact_container_dtor( factc ); \
   free( factc )

#define maren_fact_container_incref( cntr ) (((cntr)->refc)++)
#define maren_fact_container_decref( cntr ) \
   (assert( (cntr)->refc > 0 ), --((cntr)->refc) == 0)

MAREN_END_DECL
#endif /* MAREN_FACT_CONTAINER_H */
