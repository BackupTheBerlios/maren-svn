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
#include "dchk_join_node.h"

#include <stdlib.h>

#include "active_set.h"

MarenDchkJoinNode*
maren_dchk_join_node_ctor( void* where,
			   MarenDoubleCheckFn check,
			   void* data,
			   void(*del_data)(void*),
			   unsigned int l_idx,
			   unsigned int r_idx,
			   unsigned int l_sz,
			   unsigned int r_sz )
{
  MarenDchkJoinNode* ret = where ? where : malloc( sizeof(MarenDchkJoinNode) );

  if ( ret ) {
    maren_join_node_ctor( (MarenJoinNode*)ret,
			  MAREN_NT_DCHKJ,
			  l_sz, r_sz );
    
    ret->check = check;
    ret->data = data;
    ret->del_data = del_data;
    ret->l_idx = l_idx;
    ret->r_idx = r_idx;
    maren_dlist_ctor( &ret->l_list );
    maren_dlist_ctor( &ret->r_list );
  }

  return ret;
}

void
maren_dchk_join_node_dtor( MarenDchkJoinNode* node )
{
  if ( !node )
    return;

  if ( node->data && node->del_data )
    node->del_data( node->data );
  
  MarenActiveSet* acts;
  while ( (acts = (MarenActiveSet*)maren_dlist_first( &node->l_list )) ) {
    maren_dlist_iter_rm( &node->l_list, (MarenDList*)acts );
    maren_active_set_delete( acts );
  }

  while ( (acts = (MarenActiveSet*)maren_dlist_first( &node->r_list )) ) {
    maren_dlist_iter_rm( &node->r_list, (MarenDList*)acts );
    maren_active_set_delete( acts );
  }

  maren_join_node_dtor( MAREN_JOIN(node) );
}
