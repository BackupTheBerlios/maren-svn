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
#include "node.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "body_node.h"
#include "start_node.h"
#include "single_node.h"
#include "double_node.h"
#include "std_join_node.h"
#include "dchk_join_node.h"

void maren_node_set_dbg_info_va( MarenNode* node,
				 const char* txt,
				 va_list ap1 )
{
  va_list ap2;
  va_copy( ap2, ap1 );

  if ( node->dbg_info )
    free( node->dbg_info );

  size_t strLen = vsnprintf( NULL, 0, txt, ap1 ) + 1;
  if ( (node->dbg_info = malloc( strLen )) ) {
    vsprintf( node->dbg_info, txt, ap2 );
  }
  va_end( ap2 );
  va_end( ap1 );
}


void
maren_node_set_dbg_info( MarenNode* node, const char* txt, ... )
{
  va_list ap;
  va_start( ap, txt );
  maren_node_set_dbg_info_va( node, txt, ap );
  va_end( ap );
}

void
maren_node_dtor( MarenNode* node )
{
  if ( node->dbg_info )
    free( node->dbg_info );
}

void maren_node_delete( MarenNode* node )
{
  switch ( maren_node_type( node ) ) {
  case MAREN_NT_BODY:
    maren_body_node_delete( MAREN_BODY(node) );
    break;

  case MAREN_NT_START:
    maren_start_node_delete( MAREN_START(node) );
    break;

  case MAREN_NT_SINGLE:
    maren_single_node_delete( MAREN_SINGLE(node) );
    break;

  case MAREN_NT_DOUBLE:
    maren_double_node_delete( MAREN_DOUBLE(node) );
    break;

  case MAREN_NT_STDJ:
    maren_std_join_node_delete( MAREN_STDJ(node) );
    break;

  case MAREN_NT_DCHKJ:
    maren_dchk_join_node_delete( MAREN_DCHKJ(node) );
    break;

  default:
    MAREN_RT_FAIL( "Node with abstract type %d detected.",
		   maren_node_type( node ) );
  }
}
