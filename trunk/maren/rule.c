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
#include "rule.h"

#include <assert.h>

#include "start_node.h"
#include "double_node.h"
#include "dchk_join_node.h"
#include "single_node.h"
#include "body_node.h"
#include "std_join_node.h"

int
maren_rule_get_prio( struct sMarenDList* rule )
{
  MAREN_RT_ASSERT( rule != NULL,
		   _("Cannot get priority from an empty rule.") );

  MarenDList* start = maren_dlist_first( rule );
  MAREN_RT_ASSERT( start != NULL,
		   _("Cannot get priority from an empty rule.") );
  
  MarenNode* node = (MarenNode*)(maren_dlist_iter_oget( start,
							MarenStartNode,
							list ));
  while ( node ) {
    if ( maren_node_is_inner( node ) ) {
      assert( MAREN_INNER(node)->succ_num == 1 );
      node = MAREN_INNER(node)->succs->node;
    }
    else {
      assert( maren_node_is_body( node ) );
      return MAREN_BODY(node)->prio;
    }
  }

  MAREN_RT_FAIL( _("Cannot get priority from incomplete rule.") );
}

static void delete_rule_node( MarenNode* node )
{
  if ( maren_node_is_inner( node ) ) {
    MarenNodeLink *it = MAREN_INNER(node)->succs;
    MarenNodeLink *end = it + MAREN_INNER(node)->succ_num;
    while ( it < end ) {
      if ( it->type != MAREN_NL_RIGHT )
	delete_rule_node( it->node );
      it++;
    }
  }
  maren_node_delete( node );
}

void
maren_rule_dtor( MarenDList* rule )
{
  MarenDList *start;

  while ( (start = maren_dlist_first( rule )) ) {
    maren_dlist_iter_rm( rule, start );
    delete_rule_node( MAREN_NODE(maren_dlist_iter_oget( start,
							MarenStartNode,
							list )) );
  }
}

MarenStartNode*
rule_get_start( MarenDList* rule, unsigned long idx )
{
  MarenDList* ret = maren_dlist_first( rule );
  
  if ( !ret ) {
    MarenStartNode* tmp = maren_start_node_ctor( NULL, rule );
    ret = &tmp->list;
  }

  while ( idx > 0 && maren_dlist_iter_next( ret ) ) {
    ret = maren_dlist_iter_next( ret );
    idx--;
  }

  while ( idx > 0 ) {
    maren_start_node_ctor( NULL, rule );
    ret = maren_dlist_last( rule );
    idx--;
  }

  return maren_dlist_iter_oget( ret, MarenStartNode, list );
}

/** \todo Error handling. */
MarenNode*
maren_rule_add_single_check( MarenDList* rule,
			     unsigned int idx,
			     MarenSingleCheckFn check,
			     void* data,
			     void(*del_data)(void*) )
{
  MarenSingleNode *newn = maren_single_node_ctor( NULL,
						  check,
						  data,
						  del_data );
  if ( !newn )
    return NULL;

  MarenStartNode* start = rule_get_start( rule, idx );
  assert( start != NULL );

  MarenInnerNode* it = MAREN_INNER(start);
  while ( it->succ_num == 1
	  && (maren_node_is_start( it->succs->node )
	      || maren_node_is_single( it->succs->node )) )
  {
    it = MAREN_INNER(it->succs->node);
  }

  MAREN_RT_ASSERT( it->succ_num <= 1,
		   "Cannot create rule from optimized network." );
  
  maren_inner_node_insert( MAREN_INNER(newn), it, MAREN_NL_TOP );

  return MAREN_NODE(newn);
}

/** \internal The idea is, to find the last inner-node on the path
 *  down from the left and the right start-node. If we and up at the *
 *  same node from the left and the right path, the two path have *
 *  already been joined. In that case we only have to append a *
 *  double-node to the last inner node, we had found. If the last *
 *  inner-nodes for the left and the right path are differnet nodes, *
 *  the path were never joined befoer. So we have to tie the both
 *  paths * together using a new join-node.
 *
 *  \note The rule passed to the function mus meet some conditions:
 *
 *  1. Each node must have at most one successor.
 *
 *  2. When two paths have to be joined, at most one of them may have
 *  a successor. For consistency, the function complaints, if one of
 *  the nodes has a successor, which means, that one must add the
 *  body-node after the last join is done.
 */
MarenNode*
maren_rule_add_double_check( MarenDList* rule,
			     unsigned int l_idx,
			     unsigned int r_idx,
			     MarenDoubleCheckFn check,
			     void *data,
			     void(*del_data)(void*) )
{
  MarenStartNode* l_start = rule_get_start( rule, l_idx );
  assert( l_start != NULL );
  
  MarenStartNode* r_start = rule_get_start( rule, r_idx );
  assert( r_start != NULL );

  MarenInnerNode* l_end = MAREN_INNER(l_start);
  unsigned int li = 0, lsz = 1;

  assert( l_end != NULL );
  while ( l_end->succ_num > 0 &&
	  maren_node_is_inner( l_end->succs->node ) )
  {
    MAREN_RT_ASSERT( MAREN_INNER(l_end)->succ_num <= 1,
		     "Cannot create rule from optimized network." );

    switch ( l_end->succs->type ) {
    case MAREN_NL_RIGHT:
      assert( maren_node_is_join( l_end->succs->node ) );

      l_end = MAREN_INNER(l_end->succs->node);
      lsz = MAREN_JOIN(l_end)->l_sz + MAREN_JOIN(l_end)->r_sz;
      li += MAREN_JOIN(l_end)->l_sz;
      break;

    case MAREN_NL_LEFT:
      assert( maren_node_is_join( l_end->succs->node ) );

      l_end = MAREN_INNER(l_end->succs->node);
      lsz = MAREN_JOIN(l_end)->l_sz + MAREN_JOIN(l_end)->r_sz;
      break;

    default:
      l_end = MAREN_INNER(l_end->succs->node);      
    }
  }

  MarenInnerNode* r_end = MAREN_INNER(r_start);
  unsigned int ri = 0, rsz = 1;
  
  assert( r_end != NULL );
  while ( r_end->succ_num > 0 && 
	  maren_node_is_inner( r_end->succs->node ) )
  {
    MAREN_RT_ASSERT( MAREN_INNER(r_end)->succ_num <= 1,
		     "Cannot create rule from optimized network." );

    switch ( r_end->succs->type ) {
    case MAREN_NL_RIGHT:
      assert( maren_node_is_join( r_end->succs->node ) );

      r_end = MAREN_INNER(r_end->succs->node);
      rsz = MAREN_JOIN(r_end)->l_sz + MAREN_JOIN(r_end)->r_sz;
      ri += MAREN_JOIN(r_end)->l_sz;
      break;

    case MAREN_NL_LEFT:
      assert( maren_node_is_join( r_end->succs->node ) );

      r_end = MAREN_INNER(r_end->succs->node);
      rsz = MAREN_JOIN(r_end)->l_sz + MAREN_JOIN(r_end)->r_sz;
      break;

    default:
      r_end = MAREN_INNER(r_end->succs->node);
    }
  }

  if ( l_end == r_end ) {
    assert( lsz == rsz );
    MarenDoubleNode* new_node = maren_double_node_ctor( NULL,
							check,
							data,
							del_data,
							li,
							ri,
							lsz );
    assert( new_node != NULL );
    maren_inner_node_insert( MAREN_INNER(new_node),
			     l_end,
			     MAREN_NL_TOP );
    return MAREN_NODE(new_node); /* EARLY RETURN */
  }

  MAREN_RT_ASSERT( l_end->succ_num == 0 && r_end->succ_num == 0,
		   "One may not add a new join, after the body-node was "\
		   " already set." );

  MarenDchkJoinNode* new_node = maren_dchk_join_node_ctor( NULL,
							   check,
							   data,
							   del_data,
							   li,
							   ri,
							   lsz,
							   rsz );
  
  maren_inner_node_append( MAREN_NODE(new_node), l_end, MAREN_NL_LEFT );
  maren_inner_node_append( MAREN_NODE(new_node), r_end, MAREN_NL_RIGHT );

  return MAREN_NODE(new_node);
}

static unsigned int
compute_join_size( MarenInnerNode* node )
{
  switch ( maren_node_type( node ) ) {
  case MAREN_NT_START:
  case MAREN_NT_SINGLE:
    return 1;

  case MAREN_NT_DOUBLE:
    return MAREN_DOUBLE(node)->sz;

  default:
    if ( maren_node_is_join( node ) )
      return MAREN_JOIN(node)->l_sz + MAREN_JOIN(node)->r_sz;
  }

  MAREN_RT_FAIL( _("Illegal node type %d to compute join-size from.") );
}

static MarenInnerNode*
join_paths( MarenInnerNode* pending, MarenInnerNode* node )
{
  MarenNodeLink *lit, *lend;

  assert( node != NULL );
  assert( maren_node_is_inner( node ) );

  while ( node->succ_num > 0 ) {
    lit = node->succs;
    lend = lit + (node->succ_num - 1);
    while ( lit < lend ) {
      if ( lit->type != MAREN_NL_RIGHT ) {
	MAREN_RT_ASSERT( maren_node_is_inner( lit->node ),
                      _("Cannot join paths in a rule with non-inner nodes.") );
	pending = join_paths( pending, MAREN_INNER(lit->node) );
      }
      lit++;
    }
    
    if ( lend->type == MAREN_NL_RIGHT )
      return pending;		/* EARLY RETURN */

    node = MAREN_INNER(lend->node);
    MAREN_RT_ASSERT( maren_node_is_inner( node ),
		     _("Cannot join paths in a rule with non-inner nodes.") );
  }

  assert( node != NULL );
  assert( maren_node_is_inner( node ) );
  assert( node->succ_num == 0 );

  if ( !pending )
    return pending = node;	/* EARLY RETURN */

  unsigned int l_sz, r_sz;
  l_sz = compute_join_size( pending );
  r_sz = compute_join_size( node );

  MarenStdJoinNode* new_node = maren_std_join_node_ctor( NULL, l_sz, r_sz );
  
  maren_inner_node_append( MAREN_NODE(new_node), pending, MAREN_NL_LEFT );
  maren_inner_node_append( MAREN_NODE(new_node), node, MAREN_NL_RIGHT );

  return MAREN_INNER(new_node);
}

MarenNode*
maren_rule_set_body( MarenDList* rule,
		     int priority,
		     MarenActionFn action,
		     void* data,
		     void(*del_data)(void*) )
{
  MarenInnerNode* pending = NULL;
  MarenStartNode* start;

  maren_dlist_for_each( it, rule ) {
    start = maren_dlist_iter_oget( it, MarenStartNode, list );
    pending = join_paths( pending, MAREN_INNER(start) );
  }

  if ( !pending ) {
    assert( maren_dlist_empty( rule ) );
    start = maren_start_node_ctor( NULL, rule );
    pending = MAREN_INNER(start); /* Macros uses parameter more than once! */
  }

  MarenBodyNode* body = maren_body_node_ctor( NULL,
					      priority,
					      action,
					      data,
					      del_data );
  
  maren_inner_node_append( MAREN_NODE(body), pending, MAREN_NL_TOP );

  return MAREN_NODE(body);
}
