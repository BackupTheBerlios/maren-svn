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
#include <maren/rule.h>
#include <maren/dotty.h>
#include <maren/active_set.h>
#include <maren/context.h>
#include <maren/node.h>

typedef struct {
  int i[5];
} Fact;

typedef struct {
  int iidx;
  int value;
} CmpVal;

static int
check_fact_i_eq( CmpVal* value, const Fact* f )
{
  return f->i[ value->iidx ] == value->value;
}

static int
check_facts_i_eq( int* iidx, const Fact* f1, const Fact* f2 )
{
  return f1->i[ *iidx ] == f2->i[ *iidx ];
}

static int
check_facts_i_ne( int* iidx, const Fact* f1, const Fact* f2 )
{
  return f1->i[ *iidx ] != f2->i[ *iidx ];
}

static void
action_add( void* dummy, MarenActiveSet* as )
{
  
}

int main( int argc, char *argv[] )
{
  int rc = 0;
  MarenDList rule = MAREN_DLIST_INIT;
  MarenNode* node;

  CmpVal sc1 = { 0, 7 };
  node = maren_rule_add_single_check( &rule,
				      1,
				      (MarenSingleCheckFn)check_fact_i_eq,
				      &sc1, NULL );
  maren_node_set_dbg_info( node, "f.i[ %d ] == %d", sc1.iidx, sc1.value );

  CmpVal sc2 = { 1, 5 };
  node = maren_rule_add_single_check( &rule,
				      0,
				      (MarenSingleCheckFn)check_fact_i_eq,
				      &sc2, NULL );
  maren_node_set_dbg_info( node, "f.i[ %d ] == %d", sc2.iidx, sc2.value );  

  int iidx2 = 2;
  node = maren_rule_add_double_check( &rule,
				      1, 2,
				      (MarenDoubleCheckFn)check_facts_i_eq,
				      &iidx2, NULL );
  maren_node_set_dbg_info( node, "f.i[ %d ] == f.i[ %d ]", iidx2, iidx2 );

  node = maren_rule_add_double_check( &rule,
				      1, 2,
				      (MarenDoubleCheckFn)check_facts_i_ne,
				      &iidx2, NULL );
  maren_node_set_dbg_info( node, "f.i[ %d ] != f.i[ %d ]", iidx2, iidx2 );

  node = maren_rule_set_body( &rule, 0,
			      (MarenActionFn)action_add,
			      NULL, NULL );
  maren_node_set_dbg_info( node, "Summ" );

  MarenContext ctx;
  maren_context_ctor( &ctx );
  Fact f;

  maren_context_add_rule( &ctx, &rule );
  maren_ctx2dot( stdout, &ctx, NULL );

  maren_context_add_fact( &ctx, &f, NULL );

  maren_context_dtor( &ctx );

  return rc;
}
