#ifndef MAREN_NODE_H
#define MAREN_NODE_H
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

#include <stdarg.h>

#include "decls.h"

MAREN_BEGIN_DECL

/*     	       	       	   +------+   
 *     	       	       	   | Node |   
 *  	       		   +------+   
 *			      |	       
 *		     +--------+-------+
 *		     | 	       	      |	     
 *	       +-----------+   	 +----------+
 *	       | InnerNode |   	 | BodyNode |
 *	       +-----------+     +----------+
 *		     | 	       	       	       	     
 *        +----------+---+-------------+-------------+
 *        |              |             |             |  	   
 *  +-----------+ +------------+ +----------+ +------------+
 *  | StartNode | | SingleNode | | JoinNode | | DoubleNode |
 *  +-----------+ +------------+ +----------+ +------------+		     
 *	                               |
 *         +----------------+----------+-----+-------------+               
 *         |                |                |             |            
 *  +-------------+ +--------------+ +-------------+ +--------------+
 *  | StdJoinNode | | DchkJoinNode | | MapJoinNode | | HashJoinNode |
 *  +-------------+ +--------------+ +-------------+ +--------------+
 */                           

typedef enum { 
  MAREN_NT_NODE = 0,

  MAREN_NT_INNER = 1,
  MAREN_NT_BODY = 2,

  MAREN_NT_START = (1 << 2) | MAREN_NT_INNER,
  MAREN_NT_SINGLE = (2 << 2) | MAREN_NT_INNER,
  MAREN_NT_JOIN = (3 << 2) | MAREN_NT_INNER,
  MAREN_NT_DOUBLE = (4 << 2) | MAREN_NT_INNER,

  MAREN_NT_STDJ = (1 << 5) | MAREN_NT_JOIN,
  MAREN_NT_DCHKJ = (2 << 5) | MAREN_NT_JOIN,
  MAREN_NT_MAPJ = (3 << 5) | MAREN_NT_JOIN,
  MAREN_NT_HASHJ = (4 << 5) | MAREN_NT_JOIN
} MarenNodeType; 

typedef struct sMarenNode {
  MarenNodeType type;
  char* dbg_info;
} MarenNode;

#define MAREN_NODE( nodePtr ) ((MarenNode*)(nodePtr))

#define maren_node_ctor( node_ptr, node_type ) \
   ((node_ptr)->type = node_type); \
   ((node_ptr)->dbg_info = NULL)

void maren_node_dtor( MarenNode* node );

/** \internal This deletes nodes of any non-abstract type. Do \e not
 *  confuse this with something like this:
 *
 *  \code
 *  void maren_node_delete( MarenNode* node )
 *  {
 *     maren_node_dtor( node );
 *     free( node );
 *  }
 *  \endcode
 */
void maren_node_delete( MarenNode* node );

void maren_node_set_dbg_info( MarenNode* node, const char* txt, ... );

void maren_node_set_dbg_info_va( MarenNode* node,
				 const char* txt,
				 va_list ap );

#define maren_node_type( node_ptr ) (*((MarenNodeType*)(node_ptr)))


#define maren_node_is_inner( node_ptr ) \
   ((maren_node_type( node_ptr ) & 0x3 ) == MAREN_NT_INNER)
#define maren_node_is_body( node_ptr ) \
   (maren_node_type( node_ptr ) == MAREN_NT_BODY)
#define maren_node_is_start( node_ptr ) \
   (maren_node_type( node_ptr ) == MAREN_NT_START)
#define maren_node_is_single( node_ptr ) \
   (maren_node_type( node_ptr ) == MAREN_NT_SINGLE)
#define maren_node_is_join( node_ptr ) \
   ((maren_node_type( node_ptr ) & 0x1f) == MAREN_NT_JOIN)
#define maren_node_is_double( node_ptr ) \
   (maren_node_type( node_ptr ) == MAREN_NT_DOUBLE)
#define maren_node_is_stdj( node_ptr ) \
   (maren_node_type( node_ptr ) == MAREN_NT_STDJ)
#define maren_node_is_dchkj( node_ptr ) \
   (maren_node_type( node_ptr ) == MAREN_NT_DCHKJ)

MAREN_END_DECL
#endif /* MAREN_NODE_H */
