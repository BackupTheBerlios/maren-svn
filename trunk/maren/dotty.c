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
#include "dotty.h"

#include "start_node.h"
#include "dchk_join_node.h"
#include "double_node.h"
#include "context.h"

static void
print_node_name( FILE*os, const MarenNode* node )
{
  if ( node->dbg_info )
    fprintf( os, node->dbg_info );
  else
    fprintf( os, "%d@%p", node->type, node );
}

static void
print_node( FILE* os, const MarenNode* node )
{
  fprintf( os, "\t\"%p\" [ label=\"", node );

  switch ( maren_node_type( node ) ) {
  case MAREN_NT_START:
    print_node_name( os, node );
    fprintf( os, "\", shape=ellipse" );
    break;

  case MAREN_NT_BODY:
    print_node_name( os, node );
    fprintf( os, "\", shape=trapezium" );
    break;

  case MAREN_NT_DOUBLE:
    fprintf( os, "%d;%d/%d: ",
	     MAREN_DOUBLE(node)->l_idx,
	     MAREN_DOUBLE(node)->r_idx,
	     MAREN_DOUBLE(node)->sz );
    print_node_name( os, node );
    fprintf( os, "\", shape=box" );
    break;

  case MAREN_NT_STDJ:
    fprintf( os, "%d;%d: ", MAREN_JOIN(node)->l_sz, MAREN_JOIN(node)->r_sz );
    print_node_name( os, node );
    fprintf( os, "\", shape=diamond" );
    break;

  case MAREN_NT_DCHKJ:
    fprintf( os, "%d/%d;%d/%d: ", 
	     MAREN_DCHKJ(node)->l_idx,
	     MAREN_JOIN(node)->l_sz,
	     MAREN_DCHKJ(node)->r_idx,
	     MAREN_JOIN(node)->r_sz );
    print_node_name( os, node );
    fprintf( os, "\", shape=diamond" );
    break;

  default:
      print_node_name( os, node );
      fprintf( os, "\", shape=box" );
  }

  fprintf( os, " ];\n" );

  if ( maren_node_is_inner( node ) ) {
    MarenNodeLink* it = MAREN_INNER(node)->succs;
    MarenNodeLink* end = it + MAREN_INNER(node)->succ_num;

    while ( it < end ) {
      fprintf( os, "\t\"%p\" -> \"%p\";\n", node, it->node );
      it++;
    }
  }
}

typedef struct {
  FILE* os;
  const char* gname;
} CtxStartData;

static void
start_ctx( const CtxStartData* csd, const MarenContext* ctx )
{
  fprintf( csd->os, "digraph %s {\n", csd->gname ? csd->gname : "MAReN" );
}

static void
end_ctx( FILE* os, const MarenContext* ctx )
{
  fprintf( os, "}\n" );
}

void
maren_ctx2dot( FILE* os, MarenContext* ctx, const char* graphName )
{
  CtxStartData csd = { os, graphName };

  MarenContextVisitor v = {
    (void(*)(void*,const MarenContext*))start_ctx, &csd,
    (void(*)(void*,const MarenContext*))end_ctx, os,
    (void(*)(void*,const MarenNode*))print_node, os
  };
  
  maren_context_visit( ctx, &v );
}
