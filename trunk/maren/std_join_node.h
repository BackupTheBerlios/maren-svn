#ifndef MAREN_STD_JOIN_NODE_H
#define MAREN_STD_JOIN_NODE_H
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

#include "dlist.h"
#include "join_node.h"

MAREN_BEGIN_DECL

struct sMarenContext;

typedef struct {
  MarenJoinNode base;
  MarenDList l_list, r_list;
} MarenStdJoinNode;

#ifndef MAREN_CHECK_NODE_CASTS
# define MAREN_STDJ(node_ptr) \
   (assert( maren_node_is_stdj( node_ptr ) ), \
    ((MarenStdJoinNode*)(node_ptr)))
#else
# define MAREN_STDJ(node_ptr) ((MarenStdJoinNode*)(node_ptr))
#endif

MarenStdJoinNode* maren_std_join_node_ctor( void* where,
					    unsigned int l_sz,
					    unsigned int r_sz );

void maren_std_join_node_dtor( struct sMarenContext* ctx, 
			       MarenStdJoinNode* node );

#define maren_std_join_node_delete( ctx, node ) \
   maren_std_join_node_dtor( ctx, node ); \
   free( node )

MAREN_END_DECL
#endif /* MAREN_STD_JOIN_NODE_H */
