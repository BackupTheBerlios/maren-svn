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
#include "context.h"

#include <stdlib.h>
#include <string.h>

#include "start_node.h"
#include "single_node.h"
#include "active_set.h"
#include "double_node.h"
#include "dchk_join_node.h"
#include "body_node.h"
#include "std_join_node.h"
#include "fact_container.h"
#include "rule.h"
#include "hash.h"

static MarenActivePriority*
prio_upper_bound( MarenContext* ctx, int prio, bool* hit )
{
  if ( ctx->prio_num <= 0 ) {
    assert( ctx->prio_num == 0 );
    *hit = false;
    return NULL;
  }

  if ( prio <= ctx->prios->prio ) {
    *hit = prio == ctx->prios->prio;
    return ctx->prios;
  }

  size_t u;
  if ( prio > ctx->prios[ u = ctx->prio_num - 1 ].prio ) {
    *hit = false;
    return NULL;
  }
  
  int p;
  size_t l = 0, m;
  while ( u - l > 1 ) {
    m = (l + u) >> 1;
    p = ctx->prios[ m ].prio;
    if ( p > prio )
      u = m;
    else if ( p < prio )
      l = m;
    else {
      *hit = true;
      return ctx->prios + m;
    }
  }

  *hit = false;
  return ctx->prios + u;
}

void
maren_context_add_rule( MarenContext* ctx, MarenDList* rule )
{
  MarenStartNode* sn;
  int prio = maren_rule_get_prio( rule );

  if ( maren_context_has_start_hash( ctx ) ) {
    MarenDList *start = maren_dlist_first( rule );
    while ( start ) {
      sn = maren_dlist_iter_oget( start, MarenStartNode, list );
      if ( sn->hash_hint != NULL ) {
	maren_hash_key_insert( ctx->start_hash,
			       sn->hash_hint,
			       start,
			       NULL );
	start = maren_dlist_iter_rm( rule, start );
      }
      else {
	start = maren_dlist_iter_next( start );
      }
    }
  }

  if ( !maren_dlist_empty( rule ) ) {
    maren_dlist_join( &ctx->start_nodes, rule );
  }

  bool hit;
  MarenActivePriority* ap = prio_upper_bound( ctx, prio, &hit );
  
  if ( !hit ) {
    ctx->prios = realloc( ctx->prios,
			  (ctx->prio_num + 1) * sizeof(MarenActivePriority) );

    MAREN_RT_ASSERT( ctx->prios != NULL, _("Memory allocation error!") );

    if ( ap ) {
      size_t ap_idx = ap - ctx->prios;
      memmove( ctx->prios + ap_idx,
	       ctx->prios + ap_idx + 1,
	       ctx->prio_num - ap_idx );
    }
    else {
      ctx->prios[ ctx->prio_num ].prio = prio;
      ctx->prios[ ctx->prio_num ].first_active = NULL;
      ctx->prios[ ctx->prio_num ].last_active = NULL;
    }

    ctx->prio_num++;

    ctx->tmpp = realloc( ctx->tmpp,
			 ctx->prio_num * sizeof(MarenActivePriority) );
    
    MAREN_RT_ASSERT( ctx->tmpp != NULL, _("Memory allocation error!") );    
  }

  maren_dlist_ctor( rule );
}

MarenContext*
maren_context_ctor( void* where )
{
  MarenContext* ret = where ? where : malloc( sizeof(MarenContext) );

  if ( ret ) {
    maren_dlist_ctor( &ret->start_nodes );
    ret->prio_num = 0;
    ret->prios = NULL;
    ret->tmpp = NULL;
    ret->start_hash = NULL;
    ret->fact_key_hash = NULL;
  }

  return ret;
}

/** \todo Resemble all extensions due to the priority processing. */
void
maren_context_dtor( MarenContext* ctx )
{
  maren_rule_dtor( &ctx->start_nodes );

  MarenActivePriority* pit = ctx->prios;
  if ( pit ) {
    MarenActivePriority* pend = pit + ctx->prio_num;
    while ( pit < pend ) {
      MarenActiveSet* tmp;
      for ( MarenActiveSet* ait = pit->first_active; ait; ait = tmp ) {
	tmp = ait->p1;
	maren_active_set_delete( ait );
      }
      pit++;
    }
  }

  if ( ctx->start_hash ) {
    maren_hash_delete( ctx->start_hash, true );
    ctx->start_hash = NULL;
  }
}

/** \internal This macro is only to be used in \c walk_down(). Did
 *  this "horrible" macro-thing to avoid cut-copy-paste. Perhaps I
 *  will change it into an inline-function in the future. 
 *
 *  The idea ist, to call the first n-1 successors recursively. The
 *  last one will be processes in a iterative way. Assuming, that only
 *  a few nodes in a network have more than one successor, this
 *  strategy will use recursion in only a few cases.
 *
 *  \todo Make it an inline-function if it will work without \e any
 *  time-penalty.*/
#define ADD_CALL_SUCCESSORS \
   lit = MAREN_INNER(node)->succs; \
   lend = lit + (MAREN_INNER(node)->succ_num - 1); \
   while ( lit < lend ) { \
      ret += walk_down( ctx, lit->node, lit->type, acts ); \
      lit++; \
   } \
   node = lend->node; \
   link_type = lend->type
  
static int 
walk_down( MarenContext* ctx,
	   MarenNode* node,
	   MarenNodeLinkType link_type,
	   MarenActiveSet* acts )
{
  int ret = 0;
  MarenNodeLink *lit, *lend;

  do {
    switch ( maren_node_type( node ) ) {
    case MAREN_NT_START:
      ADD_CALL_SUCCESSORS;
      break;

    case MAREN_NT_SINGLE:
      if ( MAREN_SINGLE(node)->check( MAREN_SINGLE(node)->data,
				      maren_active_set_get( acts, 0 ) ) )
      {
	ADD_CALL_SUCCESSORS;
      }
      else {
	maren_active_set_delete( acts );
	goto leave_loop;
      }
      break;

    case MAREN_NT_DOUBLE:
      if ( MAREN_DOUBLE(node)->check( MAREN_DOUBLE(node)->data,
                 maren_active_set_get( acts, MAREN_DOUBLE(node)->l_idx ),
		 maren_active_set_get( acts, MAREN_DOUBLE(node)->r_idx ) ) )
      {
	ADD_CALL_SUCCESSORS;
      }
      else {
	maren_active_set_delete( acts );
	goto leave_loop;
      }
      break;

    case MAREN_NT_STDJ:
      switch ( link_type ) {
      case MAREN_NL_LEFT:
	maren_dlist_append( &(MAREN_STDJ(node)->l_list), (MarenDList*)acts );
	maren_dlist_for_each( o_set, &(MAREN_STDJ(node)->r_list) ) {
	  lit = MAREN_INNER(node)->succs;
	  lend = lit + MAREN_INNER(node)->succ_num;
	  while ( lit < lend ) {
	    walk_down( ctx,
		       lit->node,
		       lit->type,
		       maren_active_set_join( acts,
					      (MarenActiveSet*)o_set ) );
	    lit++;
	  }
	}
	break;

      case MAREN_NL_RIGHT:
	maren_dlist_append( &(MAREN_STDJ(node)->r_list), (MarenDList*)acts );
	maren_dlist_for_each( o_set, &(MAREN_STDJ(node)->l_list) ) {
	  lit = MAREN_INNER(node)->succs;
	  lend = lit + MAREN_INNER(node)->succ_num;
	  while ( lit < lend ) {
	    walk_down( ctx,
		       lit->node,
		       lit->type,
		       maren_active_set_join( (MarenActiveSet*)o_set,
					      acts ) );
	    lit++;
	  }
	}
	break;

      default:
	MAREN_RT_FAIL( "Wrong link-type %d for link to join-node.",
		       link_type );
      }
      goto leave_loop; /* case MAREN_NT_STDJ */

    case MAREN_NT_DCHKJ: {
      unsigned int o_idx;
      const void* m_fact;
      switch ( link_type ) {
      case MAREN_NL_LEFT:
	maren_dlist_append( &(MAREN_DCHKJ(node)->l_list), (MarenDList*)acts );
	m_fact = maren_active_set_get( acts, MAREN_DCHKJ(node)->l_idx );
	o_idx = MAREN_DCHKJ(node)->r_idx;

	maren_dlist_for_each( o_set, &(MAREN_DCHKJ(node)->r_list) ) {
	  if ( MAREN_DCHKJ(node)->check( MAREN_DCHKJ(node)->data,
					 m_fact,
				  maren_active_set_get( (MarenActiveSet*)o_set,
							o_idx ) ) )
	  {
	    lit = MAREN_INNER(node)->succs;
	    lend = lit + MAREN_INNER(node)->succ_num;
	    while ( lit < lend ) {
	      walk_down( ctx,
			 lit->node,
			 lit->type,
			 maren_active_set_join( acts,
						(MarenActiveSet*)o_set ) );
	      lit++;
	    }
	  }    
	}
	break;

      case MAREN_NL_RIGHT:
	maren_dlist_append( &(MAREN_DCHKJ(node)->r_list), (MarenDList*)acts );
	m_fact = maren_active_set_get( acts, MAREN_DCHKJ(node)->r_idx );
	o_idx = MAREN_DCHKJ(node)->l_idx;

	maren_dlist_for_each( o_set, &(MAREN_DCHKJ(node)->l_list) ) {
	  if ( MAREN_DCHKJ(node)->check( MAREN_DCHKJ(node)->data,
			 maren_active_set_get( (MarenActiveSet*)o_set, o_idx ),
					 m_fact ) )
	  {
	    lit = MAREN_INNER(node)->succs;
	    lend = lit + MAREN_INNER(node)->succ_num;
	    while ( lit < lend ) {
	      walk_down( ctx,
			 lit->node,
			 lit->type,
			 maren_active_set_join( (MarenActiveSet*)o_set,
						acts ) );
	      lit++;
	    }
	  }
	}
	break;
	
      default:
	MAREN_RT_FAIL( "Wrong link-type %d for link to join-node.",
		       link_type );
      }
      
      goto leave_loop;
    } /* case MAREN_NT_DCHKJ */

    case MAREN_NT_BODY:
      acts->p1 = NULL;
      acts->p2 = node;
      
      bool hit;
      MarenActivePriority* ap = prio_upper_bound( ctx,
						  MAREN_BODY(node)->prio,
						  &hit );
      assert( hit );

      if ( ap->first_active ) {
	ap->last_active->p2 = acts;
	ap->last_active = acts;
      }
      else {
	assert( ap->last_active == NULL );
	ap->first_active = ap->last_active = acts;
      }
      ret++;
      goto leave_loop;

    default:
      MAREN_RT_FAIL( "Node of abstract type %d decteced.",
		     maren_node_type( node ) );
    }
  } while ( node );
  
  leave_loop:

  return ret;
}

int maren_context_add_factc( MarenContext* ctx,
			     struct sMarenFactContainer* fact_cntr )
{
  int ret = 0;
  const MarenStartNode* sn;

  maren_fact_container_incref( fact_cntr );

  if ( fact_cntr->hash_key ) {
    assert( ctx->start_hash != NULL );

    MarenHashIter hit;
    maren_hash_search( ctx->start_hash, fact_cntr->hash_key, &hit );

    while ( maren_hash_iter_good( &hit ) ) {
      sn = maren_hash_iter_data( &hit );
      ret += walk_down( ctx,
			MAREN_NODE(sn),
			MAREN_NL_TOP,
			maren_active_set_from_fact( fact_cntr ) );
      maren_hash_iter_nexteq( &hit );
    }
  }
  else {
    maren_dlist_for_each( start, &ctx->start_nodes ) {
      sn = maren_dlist_iter_oget( start, MarenStartNode, list );
      ret += walk_down( ctx,
			MAREN_NODE(sn),
			MAREN_NL_TOP,
			maren_active_set_from_fact( fact_cntr ) );
    }
  }

  if ( maren_fact_container_decref( fact_cntr ) ) {
    maren_fact_container_del( fact_cntr );
  }

  return ret;
}

int
maren_context_add_fact( MarenContext* ctx,
			const void* fact,
			void (*del_fact)(void*) )
{
  MarenFactContainer* fctnr = maren_fact_container_ctor( fact, del_fact );

  if ( maren_context_has_start_hash( ctx ) ) {
    fctnr->hash_key = ctx->fact_key_hash( fact, &fctnr->del_hash_key );
  }

  return maren_context_add_factc( ctx, fctnr );
}

/** \internal This macro is only to be used in \c remove_fact(). Did
 *  this "horrible" macro-thing to avoid cut-copy-paste. Perhaps I
 *  will change it into an inline-function in the future.
 *
 *  The idea ist, to call the first n-1 successors recursively. The
 *  last one will be processes in a iterative way. Assuming, that only
 *  a few nodes in a network have more than one successor, this
 *  strategy will use recursion in only a few cases. 
 *
 *  \todo Make it an inline-function if it will work without \e any
 *  time-penalty.*/
#define RM_CALL_SUCCESSORS \
   lit = MAREN_INNER(node)->succs; \
   lend = lit + (MAREN_INNER(node)->succ_num - 1); \
   while ( lit < lend ) { \
      remove_fact( lit->node, lit->type, idx, fact ); \
      lit++; \
   } \
   node = lend->node; \
   link_type = lend->type

static void
remove_fact( MarenNode* node,
	     MarenNodeLinkType link_type,
	     unsigned int idx,
	     const void *fact )
{
  MarenNodeLink *lit, *lend;

  while ( node ) {
    switch ( maren_node_type( node ) ) {
    case MAREN_NT_START:
      assert( idx == 0 );
      /* fall through! */

    case MAREN_NT_DOUBLE:
      RM_CALL_SUCCESSORS;
      break;

    case MAREN_NT_SINGLE:
      assert( idx == 0 );
      if ( MAREN_SINGLE(node)->check( MAREN_SINGLE(node)->data, fact ) ) {
	RM_CALL_SUCCESSORS;
      }
      else {
	return;
      }
      break;

    case MAREN_NT_STDJ: {
      MarenDList* acts;
      MarenActiveSet* kill_set;

      switch ( link_type ) {
      case MAREN_NL_LEFT:
	acts = maren_dlist_first( &(MAREN_STDJ(node)->l_list) );
	while ( acts ) {
	  if ( maren_active_set_get( (MarenActiveSet*)acts, idx ) == fact ) {
	    kill_set = (MarenActiveSet*)acts;
	    kill_set->facts[ idx ]->removed = true;
	    acts = maren_dlist_iter_rm( &(MAREN_STDJ(node)->l_list), acts );
	    maren_active_set_delete( kill_set );
	  }
	  else {
	    acts = maren_dlist_iter_next( acts );
	  }
	}
	break;

      case MAREN_NL_RIGHT:
	acts = maren_dlist_first( &(MAREN_STDJ(node)->r_list) );
	while ( acts ) {
	  if ( maren_active_set_get( (MarenActiveSet*)acts, idx ) == fact ) {
	    kill_set = (MarenActiveSet*)acts;
	    kill_set->facts[ idx ]->removed = true;
	    acts = maren_dlist_iter_rm( &(MAREN_STDJ(node)->r_list), acts );
	    maren_active_set_delete( kill_set );
	  }
	  else {
	    acts = maren_dlist_iter_next( acts );
	  }
	}
	break;

      default:
	MAREN_RT_FAIL( "Wrong link-type %d for linkt to join-node.",
		       link_type );
      }

      RM_CALL_SUCCESSORS;
    } break; /* case MAREN_NT_STDJ */

    case MAREN_NT_DCHKJ: {
      MarenDList* acts;
      MarenActiveSet *kill_set;

      switch ( link_type ) {
      case MAREN_NL_LEFT:
	acts = maren_dlist_first( &(MAREN_DCHKJ(node)->l_list) );
	while ( acts ) {
	  if ( maren_active_set_get( (MarenActiveSet*)acts, idx ) == fact ) {
	    kill_set = (MarenActiveSet*)acts;
	    kill_set->facts[ idx ]->removed = true;
	    acts = maren_dlist_iter_rm( &(MAREN_DCHKJ(node)->l_list), acts );
	    maren_active_set_delete( kill_set );
	  }
	  else {
	    acts = maren_dlist_iter_next( acts );
	  }
	}
	break;
	
      case MAREN_NL_RIGHT:
	acts = maren_dlist_first( &MAREN_DCHKJ(node)->r_list );
	while ( acts ) {
	  if ( maren_active_set_get( (MarenActiveSet*)acts, idx ) == fact ) {
	    kill_set = (MarenActiveSet*)acts;
	    kill_set->facts[ idx ]->removed = true;
	    acts = maren_dlist_iter_rm( &MAREN_DCHKJ(node)->r_list, acts );
	    maren_active_set_delete( kill_set );
	  }
	  else {
	    acts = maren_dlist_iter_next( acts );
	  }
	}
	idx += MAREN_JOIN(node)->l_sz;
	break;

      default:
	MAREN_RT_FAIL( "Wrong link-type %d for linkt to join-node.",
		       link_type );
      }

      RM_CALL_SUCCESSORS;
    } break; /* case MAREN_NT_DCHKJ */

    case MAREN_NT_BODY:
      return;

    default:
      MAREN_RT_FAIL( "Node of abstract type %d detetcted.", node->type );
    }
  }
}

void
maren_context_rm_fact( MarenContext* ctx, const void* fact )
{
  const MarenStartNode* sn;
  void (*del_hash_key)( void* ) = NULL;
  const void* hash_key = NULL;
  
  if ( maren_context_has_start_hash( ctx ) ) {
    hash_key = ctx->fact_key_hash( fact, &del_hash_key );
  }

  if ( hash_key ) {
    assert( ctx->start_hash != NULL );
    
    MarenHashIter hit;
    maren_hash_search( ctx->start_hash, hash_key, &hit );

    while ( maren_hash_iter_good( &hit ) ) {
      sn = maren_hash_iter_data( &hit );
      remove_fact( MAREN_NODE(sn), MAREN_NL_TOP, 0,fact );
    }

    if ( del_hash_key ) {
      del_hash_key( (void*)hash_key );
    }
  }
  else {
    maren_dlist_for_each( start, &ctx->start_nodes ) {
      sn = maren_dlist_iter_oget( start, MarenStartNode, list );
      remove_fact( MAREN_NODE(sn), MAREN_NL_TOP, 0, fact );
    }
  }
}

void
maren_context_fire_activated( MarenContext* ctx )
{
  MarenBodyNode* body;
  MarenActiveSet *tmp, *it;
  MarenActivePriority* prio = ctx->prios;
  MarenActivePriority* pend = prio + ctx->prio_num;
  ctx->prios = ctx->tmpp;
  ctx->tmpp = prio;

  while ( prio < pend ) {
    it = prio->first_active;
    while ( it ) {
      body = MAREN_BODY(it->p2);
      body->action( body->data, ctx, it );
      tmp = it->p1;
      maren_active_set_delete( it );
      it = tmp;
    }
    prio->first_active = prio->last_active = NULL;
    prio++;
  }
}

bool
maren_context_has_active( MarenContext* ctx )
{
  MarenActivePriority* prio = ctx->prios;
  MarenActivePriority* pend = prio + ctx->prio_num;

  while ( prio < pend ) {
    if ( prio->first_active ) {
      assert( prio->last_active != NULL );
      return true;
    }
    prio++;
  }

  return false;
}

void
maren_context_set_start_hash( MarenContext* ctx,
			      const void*(*fact_key)(const void*,
						     void(**)(void*)),
			      unsigned long (*hash_fn)(const void*),
			      int (*hash_cmp)(const void*, const void*),
			      size_t start_buckets )
{
  MAREN_RT_ASSERT( ctx->start_hash == NULL,
		   _("It is not alowed to reset the start-hash configuration "\
		     "of a context.") );

  ctx->start_hash = maren_hash_create( NULL,
				       hash_fn,
				       hash_cmp,
				       start_buckets );
  ctx->fact_key_hash = fact_key;
}

static void
visit_path( MarenContextVisitor* v, const MarenNode* node )
{
  while ( node ) {
    if ( v->node_fn )
      v->node_fn( v->node_fn_data, node );

    if ( maren_node_is_inner( node ) ) {
      MarenNodeLink* lit = MAREN_INNER(node)->succs;
      MarenNodeLink* lend = lit + (MAREN_INNER(node)->succ_num - 1);

      while ( lit < lend ) {
	if ( lit->type != MAREN_NL_RIGHT ) {
	  visit_path( v, lit->node );
	}
	lit++;
      }

      node = lend->type == MAREN_NL_RIGHT ? NULL : lend->node;
    }
    else {
      break;
    }
  }
}

void
maren_context_visit( MarenContext* ctx, MarenContextVisitor* visitor )
{
  if ( visitor->start_context )
    visitor->start_context( visitor->ctx_start_data, ctx );
  
  MarenStartNode* sn;
  maren_dlist_for_each( start, &ctx->start_nodes ) {
    sn = maren_dlist_iter_oget( start, MarenStartNode, list );
    visit_path( visitor, MAREN_NODE(sn) );
  }

  if ( maren_context_has_start_hash( ctx ) ) {
    MarenHashIter it = MAREN_HASH_ITER_INIT;
    for ( maren_hash_iter_begin( &it, ctx->start_hash );
	  maren_hash_iter_good( &it );
	  maren_hash_iter_next( &it ) )
    {
      visit_path( visitor,
		  maren_hash_iter_data( &it ) );
    }
  }

  if ( visitor->end_context )
    visitor->end_context( visitor->ctx_end_data, ctx );
}


/** \mainpage
 *
 *  \section copying Copying
 *
 *  Copyright (c) 2004 Marcus Perlick \n
 *  All rights reserved. 
 *
 * \section intro Introduction
 *
 * So, what is a rule-engine, then?  I heard someone saying, that one
 * should imagine it as a very clever implementation for very long \c
 * if-then-else cascades.  This is a nice hint to get the first idea
 * of the whole thing, but in reality, a rule-engine is a lot more
 * than that.
 *
 * And why is this rule-engine called MAReN?  The first time, I came
 * across rule engines, it was an engine based on the Rete algorithm
 * (todo: give the reference).  And I wondered, why the condition of
 * one rule always is a (todo: dis-/conjunction; which one) of
 * predicates.  I had liked it more, if there could be general boolean
 * expressions about predicates.  So I tried to design a rule system
 * --- inspired by the Rete algorithm --- that lets one give general
 * boolean expressions as a rules condition.  To emphasize, that this
 * system is \e not a Rete implementation, I called it <em>Maren Ain't
 * a Rete Network</em> short <em>MAReN</em>!  Later, I gave up the
 * idea of general boolean expressions, because they would make the
 * whole thing much more complex without extending the expressiveness
 * of the system.  Today, MAReN is based on the Rete algorithm.
 *
 * For the rest of the text I beg you to excuse my lax wrinting, but
 * there are enough scientific, brilliant, formal writings about rule
 * engines.
 *
 * \subsection rule Rules
 * 
 * To understand, what you can do with this rule-engine, I suppose
 * that you first try to understand, what a rule is.  A rule is
 * something, that consists of exactly two things:
 *
 * -# The rules \e condition.
 * -# The rules \e body.
 *
 * The \e body is something that can be done under certain
 * circumstances.  The \e condition tells us, what circumstance have
 * to be met, to execute the body. An example of a rule could be:
 *
 * \verbatim If there is something that is a house  and
 that house is buring                   and
 there is something that is a phone     and
 that phone is not in the house
 
 then
 
 call the firefighters using that phone.\endverbatim
 *
 * The part above the \c then is the conditon and the part below the
 * \c then is the body.  Sounds quite easy, eh?  To make it a bit more
 * complicated, we could write it down in a more formal way:
 *
 * \code
 * (?a, is_house( a ), is_burning( a ), ?b, is_phone( b ), not_in( b, a ))
 * -->
 * ( b.call( firefighters ) )
 * \endcode
 *
 * Ahhh, that looks more like something for real programmers! And what
 * is quite clear from this notation is, that the rule is about two
 * things \f$a\f$ and \f$b\f$. In the future we will call those
 * subjects of a rule \e facts. Then there are some predicates like \c
 * is_house() and so on, that are applied to the facts. As we see,
 * there are unary and binary prediactes. The rule will only be
 * exectued, if all predicates evaluate to \c true.
 *
 * OK, that's all I have to say about rules in this place. Even, if
 * there are some urgent questions. Some of them might be explained in
 * the next section.
 *
 * \subsection Context
 *
 * Now, that you understood, what a rule is, you might think: "Oh, if
 * I have a rule about two facts, as in the example above, I can give
 * two facts to the rule and it will do something or not."
 *
 * If that would be all about rule-engines, there wouldn't be
 * rule-engines. Testing exactly \f$n\f$ facts against a rule that
 * takes \f$n\f$ facts can be so easily implemented with programming
 * languages like C, C++, Java, C#, Prolog, Haskell or whatever you
 * want, that there is no need to have a special software system for
 * that. No, the secret lies in the <em>"If there is
 * something"</em>-question in the text-version of the rule! The real
 * question is: Where are all those somethings, that have to be
 * considered? The answer is:
 *
 * They live in the \b context.\n
 *
 * The context is the thing, that defines all our knowledge and
 * wisdom. And not only that, it also knows about all the rules of our
 * little world. This knowledge gives the context a fascinating
 * ability: Given a set of rules, it knows which rules' conditions are
 * fulfilled by which sets of facts. If a rules condition is fulfilled,
 * we say the rule is \e activated.  That means, if the rule \f$R\f$
 * is activated by a set of facts \f$(f_1, f_2, f_3)\f$ and another
 * set \f$(f_{54}, f_{2635}, f_6)\f$ the context knows about that. It
 * knows about all sets that activate a rule and for each of this sets
 * it knows which rule it activates. And not enough with
 * that. Considering that the facts arrived in a sequential order at
 * the context, the context also knows in which order the sets
 * activated their rules.
 *
 * \note Activating a rule does \e not mean to execute it's body!
 *
 * From the practical point of view, the context is a thing that can
 * be equipped with a set of rules. Being done with that, one can feed
 * the context with facts - one after another. And at any time, the
 * context knows which sets of facts activated which rules in which
 * order.
 *
 * This is the point at which we see, that a rule-engine -
 * respectively it's context - is a quite clever thing. But is it
 * useful in some way, also? The answer is: Of course, it can be! And
 * it is useful, because we can not only activate rules, but we also
 * can execute (or \e fire) rules.
 */
