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
#include "start_node.h"

#include <stdlib.h>

MarenStartNode*
maren_start_node_ctor( void* where, MarenDList* rule )
{
  MarenStartNode* ret = where ? where : malloc( sizeof(MarenStartNode) );

  if ( ret ) {
    maren_inner_node_ctor( (MarenInnerNode*)ret, MAREN_NT_START );
    maren_dlist_ctor( &ret->list );
    maren_dlist_append( rule, &ret->list );
  }

  return ret;
}

void
maren_start_node_dtor( MarenStartNode* node )
{
  if ( node )
    maren_inner_node_dtor( MAREN_INNER(node) );
}

MarenStartNode*
maren_start_node_prev( MarenStartNode* node )
{
  char* dlNode = (char*)node->list.p;
  return (MarenStartNode*)(dlNode - offsetof(MarenStartNode, list));
}

MarenStartNode*
maren_start_node_next( MarenStartNode* node )
{
  char* dlNode = (char*)node->list.n;
  return (MarenStartNode*)(dlNode - offsetof(MarenStartNode, list));
}
