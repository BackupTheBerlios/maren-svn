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
#include "inner_node.h"

#include <stdlib.h>

void
maren_inner_node_dtor( MarenInnerNode* node )
{
  maren_node_dtor( MAREN_NODE(node) );
}

void
maren_inner_node_append( MarenNode* new_node,
			 MarenInnerNode* after_this,
			 MarenNodeLinkType link_type )
{
  MarenNodeLink* new_links = realloc( after_this->succs,
				      (after_this->succ_num + 1)
				      * sizeof(MarenNodeLink) );

  MAREN_RT_ASSERT( new_links != NULL, "Memory reallocation failed." );
  
  MarenNodeLink* link = new_links + after_this->succ_num;
  link->type = link_type;
  link->node = new_node;
  
  after_this->succ_num++;
  after_this->succs = new_links;
}

void
maren_inner_node_insert( MarenInnerNode* new_node,
			 MarenInnerNode* behind_this,
			 MarenNodeLinkType link_type )
{
  MAREN_RT_ASSERT( new_node->succ_num == 0,
		   "The new node must not have successors." );

  new_node->succ_num = behind_this->succ_num;
  new_node->succs = behind_this->succs;
  
  behind_this->succ_num = 0;
  behind_this->succs = NULL;

  maren_inner_node_append( MAREN_NODE(new_node), behind_this, link_type );
}
