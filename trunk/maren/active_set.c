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
#include "active_set.h"

#include <stdlib.h>
#include <string.h>

#include "fact_container.h"

MarenActiveSet*
maren_active_set_from_fact( MarenFactContainer* fact )
{
  MarenActiveSet* ret = malloc( sizeof(MarenActiveSet)
				+ sizeof(MarenFactContainer*) );

  MAREN_RT_ASSERT( ret != NULL, "Malloc failed" );

  // ret->p1 = ret->p2 = NULL;
  ret->size = 1;
  ret->facts[ 0 ] = fact;
  fact->refc++;

  return ret;
}

MarenActiveSet*
maren_active_set_join( const MarenActiveSet* as1, const MarenActiveSet* as2 )
{
  MarenActiveSet* ret = malloc( sizeof(MarenActiveSet)
				+ (as1->size + as2->size)
				* sizeof(MarenFactContainer*) );
  
  MAREN_RT_ASSERT( ret != NULL, "Malloc failed" );

  memcpy( ret->facts,
	  as1->facts,
	  as1->size * sizeof(MarenFactContainer*) );

  memcpy( ret->facts + as1->size,
	  as2->facts,
	  as2->size * sizeof(MarenFactContainer*) );

  ret->size = as1->size + as2->size;
  MarenFactContainer **it = ret->facts, **end = it + ret->size;
  while ( it < end ) {
    assert( *it != NULL );
    (*it)->refc++;
    it++;
  }

  return ret;
}

void
maren_active_set_delete( MarenActiveSet* as )
{
  MarenFactContainer **it = as->facts, **end = it + as->size;
  while ( it < end ) {
    if ( maren_fact_container_decref( *it ) ) {
      maren_fact_container_del( *it );
    }
    it++;
  }
}
