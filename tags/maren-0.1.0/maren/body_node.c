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
#include "body_node.h"

#include <stdlib.h>

MarenBodyNode*
maren_body_node_ctor( void* where,
		      int priority,
		      MarenActionFn action,
		      void* data,
		      void (*del_data)(void*) )
{
  MarenBodyNode* ret = where ? where : malloc( sizeof(MarenBodyNode) );

  if ( ret ) {
    maren_node_ctor( (MarenNode*)ret, MAREN_NT_BODY );
    ret->prio = priority;
    ret->action = action;
    ret->data = data;
    ret->del_data = del_data;
  }

  return ret;
}

void
maren_body_node_dtor( MarenBodyNode* node )
{
  if ( node ) {
    if ( node->data && node->del_data )
      node->del_data( node->data );

    maren_node_dtor( MAREN_NODE(node) );
  }
}
