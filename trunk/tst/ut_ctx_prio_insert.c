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
#include <maren/maren.h>

/* Need this include only for unti-test reasons, because we wanna
 * inspect the context data structure. */
#include <maren/context.h>

static void dummy_action( void* data,
			  MarenContext* ctx, 
			  const MarenActiveSet* acts )
{
  
}

int main( int argc, char *argv[] )
{
  int rc = 0;
  int i, p;
  MarenContext ctx = MAREN_CONTEXT_INIT;
  MarenDList rule = MAREN_DLIST_INIT;

  maren_rule_set_body( &rule, 0, dummy_action, NULL, NULL );
  maren_context_add_rule( &ctx, &rule );

  maren_rule_set_body( &rule, 2, dummy_action, NULL, NULL );
  maren_context_add_rule( &ctx, &rule );

  maren_rule_set_body( &rule, -2, dummy_action, NULL, NULL );
  maren_context_add_rule( &ctx, &rule );

  maren_rule_set_body( &rule, -1, dummy_action, NULL, NULL );
  maren_context_add_rule( &ctx, &rule );

  maren_rule_set_body( &rule, 1, dummy_action, NULL, NULL );
  maren_context_add_rule( &ctx, &rule );

  maren_rule_set_body( &rule, 3, dummy_action, NULL, NULL );
  maren_context_add_rule( &ctx, &rule );

  maren_rule_set_body( &rule, -3, dummy_action, NULL, NULL );
  maren_context_add_rule( &ctx, &rule );

  assert( ctx.prio_num == 7 );
  for ( p = -3, i = 0; i < ctx.prio_num; p++ , i++ ) {
    assert( ctx.prios[ i ].prio == p );
  }

  maren_context_add_fact( &ctx, "Hello, world!", NULL );

  

  return rc;
}
