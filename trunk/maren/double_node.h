#ifndef MAREN_DOUBLE_NODE_H
#define MAREN_DOUBLE_NODE_H
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
#include "node_functions.h"

MAREN_BEGIN_DECL

typedef struct {
  MarenInnerNode base;
  MarenDoubleCheckFn check;
  void* data;
  void (*del_data)(void*);
  unsigned int l_idx, r_idx;
  unsigned int sz;
} MarenDoubleNode;

#ifdef MAREN_CHECK_NODE_CASTS
# define MAREN_DOUBLE(node_ptr) \
   (assert( maren_node_is_double( node_ptr ) ), \
    ((MarenDoubleNode*)(node_ptr)))
#else
# define MAREN_DOUBLE(node_ptr) ((MarenDoubleNode*)(node_ptr))
#endif

/** \brief Create a double-node. */
MarenDoubleNode* maren_double_node_ctor( void* where,
					 MarenDoubleCheckFn check,
					 void* data,
					 void (*del_data)(void*),
					 unsigned int l_idx,
					 unsigned int r_idx,
					 unsigned int sz );

void maren_double_node_dtor( MarenDoubleNode* node );
#define maren_double_node_delete( node ) \
   maren_double_node_dtor( node ); \
   free( node )

MAREN_END_DECL
#endif /* MAREN_DOUBLE_NODE_H */
