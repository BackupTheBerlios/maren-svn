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
#include "std_join_node.h"

#include <stdlib.h>

#include "active_set.h"

MarenStdJoinNode*
maren_std_join_node_ctor( void* where, unsigned int l_sz, unsigned int r_sz )
{
  MarenStdJoinNode* ret = where ? where : malloc( sizeof(MarenStdJoinNode) );

  if ( ret ) {
    maren_join_node_ctor( (MarenJoinNode*)ret,
			  MAREN_NT_STDJ,
			  l_sz, r_sz );
    
    maren_dlist_ctor( &ret->l_list );
    maren_dlist_ctor( &ret->r_list );
  }

  return ret;
}

void
maren_std_join_node_dtor( MarenStdJoinNode* node )
{
  if ( !node )
    return;
  
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
