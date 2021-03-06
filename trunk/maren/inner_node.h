#ifndef MAREN_INNER_NODE_H
#define MAREN_INNER_NODE_H
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

#include <stddef.h>

#include "node.h"

MAREN_BEGIN_DECL

typedef enum {
  MAREN_NL_TOP,
  MAREN_NL_LEFT,
  MAREN_NL_RIGHT
} MarenNodeLinkType;

typedef struct {
  MarenNodeLinkType type;
  MarenNode* node;
} MarenNodeLink;

typedef struct {
  MarenNode base;
  size_t succ_num;
  MarenNodeLink* succs;
} MarenInnerNode;

#ifdef MAREN_CHECK_NODE_CASTS
# define MAREN_INNER(node_ptr) \
   (assert( maren_node_is_inner( node_ptr ) ), \
    ((MarenInnerNode*)(node_ptr)))
#else
# define MAREN_INNER(node_ptr) ((MarenInnerNode*)(node_ptr))
#endif

#define maren_inner_node_ctor( node_ptr, node_type ) \
   maren_node_ctor( (MarenNode*)(node_ptr), node_type ); \
   (node_ptr)->succ_num = 0; \
   (node_ptr)->succs = NULL

/** \internal The destructor of \c MarenInnerNode does \e not cascade,
 *  i.e it won't delete any successor nodes. The idea is, that the
 *  d'tor usually will be used to delete rules or whole networks. In
 *  such a network, nodes can be reached via more than one other
 *  node. So if this d'tor would cascade, onewould take the risk of
 *  calling other d'tors twice on the same address. */
void maren_inner_node_dtor( MarenInnerNode* node );

void maren_inner_node_append( MarenNode* new_node,
			      MarenInnerNode* after_this,
			      MarenNodeLinkType link_type );

void maren_inner_node_insert( MarenInnerNode* new_node,
			      MarenInnerNode* behind_this,
			      MarenNodeLinkType link_type );

MAREN_END_DECL
#endif /* MAREN_INNER_NODE_H */
