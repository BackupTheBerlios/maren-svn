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
#include <string.h>
#include <maren/maren.h>
#include <maren/hash.h>

static const void*
get_fact_key( const void* fact, void(**del_key)(void*) )
{
  del_key = NULL;
  return fact;
}

unsigned long
hash_fact( const void* fact )
{
  return maren_binary_hash( 0, fact, strlen( (const char*)fact ) );
}

int main( int argc, char *argv[] )
{
  int rc = 0;
  MarenContext ctx = MAREN_CONTEXT_INIT;
  MarenDList rule = MAREN_DLIST_INIT;
  MarenNodePtr node;

  maren_context_set_start_hash( &ctx,
				get_fact_key,
				hash_fact,
				(int(*)(const void*,const void*))strcmp,
				0 );
  
  node = maren_rule_set_start_hash_hint( &rule,
					 0,
					 "node1",
					 NULL );
  maren_node_set_dbg_info( node, "node1" );
  

  return rc;
}
