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
#include "fact_container.h"

#include <stdlib.h>

MarenFactContainer*
maren_fact_container_ctor( const void* fact, void (*del_fact)(void*) )
{
  MarenFactContainer* ret = malloc( sizeof(MarenFactContainer) );

  MAREN_RT_ASSERT( ret != NULL, "Malloc failed" );

  ret->refc = 0;
  ret->fact = fact;
  ret->del_fact = del_fact;
  ret->hash_key = NULL;
  ret->del_hash_key = NULL;
  ret->removed = false;
  
  return ret;
}

void
maren_fact_container_dtor( MarenFactContainer* factc )
{
  if ( factc->fact && factc->del_fact )
    factc->del_fact( (void*)(factc->fact) );

  if ( factc->hash_key && factc->del_hash_key )
    factc->del_hash_key( (void*)(factc->hash_key) );
}
