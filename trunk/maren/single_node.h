#ifndef MAREN_SINGLE_NODE_H
#define MAREN_SINGLE_NODE_H
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
  MarenSingleCheckFn check;
  void* data;
  void (*del_data)(void*);
} MarenSingleNode;

#ifdef MAREN_CHECK_NODE_CASTS
# define MAREN_SINGLE(node_ptr) \
   (assert( maren_node_is_single( node_ptr ) ), \
    ((MarenSingleNode*)(node_ptr)))
#else
# define MAREN_SINGLE(node_ptr) ((MarenSingleNode*)(node_ptr))
#endif

MarenSingleNode* maren_single_node_ctor( void* where,
					 MarenSingleCheckFn check,
					 void* data,
					 void (*del_data)(void*) );

void maren_single_node_dtor( MarenSingleNode* node );
#define maren_single_node_delete( node ) \
   maren_single_node_dtor( node ); \
   free( node )

MAREN_END_DECL
#endif /* MAREN_SINGLE_NODE_H */
