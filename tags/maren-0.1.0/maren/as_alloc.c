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
#include "as_alloc.h"

#include "active_set.h" 

#if MAREN_ACTIVE_SET_ALLOC == 1
MarenActiveSetAlloc*
maren_active_set_alloc_ctor( MarenActiveSetAlloc* where,
			     size_t as_size_max,
			     size_t max_sets_per_slot )
{
  MarenActiveSetAlloc* ret = where 
    ? where
    : malloc( sizeof(MarenActiveSetAlloc) );

  if ( ret ) {
    ret->as_size_end = as_size_max + 1;
    ret->slots = malloc( ret->as_size_end * sizeof(MarenActiveSetAllocSlot) );
    if ( ret->slots ) {
      for ( size_t i = 0; i < ret->as_size_end; i++ ) {
	ret->slots[ i ].size = sizeof(MarenActiveSet) 
	  + i * sizeof(MarenFactContainer*);

	ret->slots[ i ].count = 0;
	ret->slots[ i ].mem = NULL;
      }
    }
    else if ( !where ) {
      free( ret );
      ret = NULL;
    }
  }

  return ret;
}

void
maren_active_set_alloc_dtor( MarenActiveSetAlloc* alloc )
{
  if ( !alloc )
    return;

  MarenActiveSetAllocSlot* it = alloc->slots;
  MarenActiveSetAllocSlot* end = it + alloc->as_size_end;
  while ( it < end ) {
    assert( (it->count == 0) == (it->mem == NULL) );
    void* tmp;
    while ( it->mem ) {
      tmp = *(void**)(it->mem);
      free( it->mem );
      it->mem = tmp;
    }
    it++;
  }
  free( alloc->slots );
}

void*
maren_active_set_alloc_alloc( MarenActiveSetAlloc* alloc, size_t sz )
{
  void* ret = NULL;

  if ( sz < alloc->as_size_end ) {
    MarenActiveSetAllocSlot* slot = alloc->slots + sz;
    assert( (slot->count == 0) == (slot->mem == NULL) );
    if ( slot->mem ) {
      ret = slot->mem;
      slot->mem = *(void**)ret;
      slot->count--;
    }
    else
      ret = malloc( slot->size );
  }
  else
    ret = malloc( sizeof(MarenActiveSet) + sz * sizeof(MarenFactContainer*) );

  return ret;
}

void
maren_active_set_alloc_free( MarenActiveSetAlloc* alloc, MarenActiveSet* as )
{
  if ( as->size < alloc->as_size_end ) {
    MarenActiveSetAllocSlot* slot = alloc->slots + as->size;
    assert( (slot->count == 0) == (slot->mem == NULL) );
    
    if ( slot->count < alloc->slot_max ) {
      *(void**)as = slot->mem;
      slot->mem = as;
      slot->count++;
    }
    else
      free( as );
  }
  else {
    free( as );
  }
}
#endif // MAREN_ACTIVE_SET_ALLOC == 1
