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

typedef enum {
  LDC_COMPLETE,
  LDC_FIRST,
  LDC_INTERMEDIATE,
  LDC_LAST
} LDCState;

typedef struct {
  char* a_num;
  char* b_num;
  long start;
  unsigned int duration;
  LDCState ldc;
} CDR;

static void
print_cdr( FILE* os, const CDR* cdr )
{
  fprintf( os, "%s %s %ld %u %d",
	   cdr->a_num,
	   cdr->b_num,
	   cdr->start,
	   cdr->duration,
	   cdr->ldc );
}

void del_cdr( CDR* cdr )
{
  printf( "Delete CDR:" );
  print_cdr( stdout, cdr );
  putchar( '\n' );
}

static bool
cdr_end1_start2_match( void* dummy, const CDR* cdr1, const CDR* cdr2 )
{
  long cdr1_end = cdr1->start + cdr1->duration;
  long d = cdr2->start - cdr1_end;
  return d >= 0 && d < 2;
}

static void
assemble_start_end( void* data, MarenContext* ctx, const MarenActiveSet* acts )
{
  CDR *start = (CDR*)maren_active_set_get( acts, 0 );
  CDR *end = (CDR*)maren_active_set_get( acts, 1 );

  putchar( '(' );
  print_cdr( stdout, start );
  printf( ") + (" );
  print_cdr( stdout, end );
  printf( ") = (" );

  start->duration += end->duration;
  start->ldc = LDC_COMPLETE;

  print_cdr( stdout, start );
  printf( ") !\n" );

  maren_context_rm_fact( ctx, start );
  maren_context_rm_fact( ctx, end );
}

/** \todo reinsert fact-containers instead of facts. */
static void
assemble_start_intrm( void* data, 
		      MarenContext* ctx,
		      const MarenActiveSet* acts )
{
  CDR *start = (CDR*)maren_active_set_get( acts, 0 );
  CDR *intrm = (CDR*)maren_active_set_get( acts, 1 );

  putchar( '(' );
  print_cdr( stdout, start );
  printf( ") + (" );
  print_cdr( stdout, intrm );
  printf( ") = (" );

  start->duration += intrm->duration;

  print_cdr( stdout, start );
  printf( ") ->\n" );

  maren_context_rm_fact( ctx, start );
  maren_context_rm_fact( ctx, intrm );
  
  maren_context_add_factc( ctx, acts->facts[ 0 ] );
}


int main( int argc, char *argv[] )
{
  int rc = 0;
  MarenNodePtr node;
  MarenContext ctx = MAREN_CONTEXT_INIT;
  MarenDList rule = MAREN_DLIST_INIT;

  // Rule: 0:ldc-start & 1:ldc-end & 0,1:end-to-start match &
  //       0,1: a_num eq & 0,1: b_num eq
  MarenStdSingleTestData ldc_is_start = {offsetof( CDR, ldc ), { LDC_FIRST }};
  node = maren_rule_add_single_check( &rule, 0,
				      maren_std_single_test_int_eq,
				      &ldc_is_start, NULL );
  maren_node_set_dbg_info( node, "LDC-start" );

  MarenStdSingleTestData ldc_is_end = {offsetof( CDR, ldc ), { LDC_LAST }};
  node = maren_rule_add_single_check( &rule, 1,
				      maren_std_single_test_int_eq,
				      &ldc_is_end, NULL );
  maren_node_set_dbg_info( node, "LDC-end" );
  
  node
    = maren_rule_add_double_check( &rule,
				   0, 1,
				   (MarenDoubleCheckFn)cdr_end1_start2_match,
				   NULL, NULL );
  maren_node_set_dbg_info( node, "end-to-start match" );

  MarenStdDoubleTestData a_num_eq = { offsetof( CDR, a_num ),
				      offsetof( CDR, a_num ) };
  node = maren_rule_add_double_check( &rule,
				      0, 1,
				      maren_std_double_test_cstr_eq,
				      &a_num_eq, NULL );
  maren_node_set_dbg_info( node, "a_num equal" );

  MarenStdDoubleTestData b_num_eq = { offsetof( CDR, b_num ),
				      offsetof( CDR, b_num ) };
  node = maren_rule_add_double_check( &rule,
				      0, 1,
				      maren_std_double_test_cstr_eq,
				      &b_num_eq, NULL );
  maren_node_set_dbg_info( node, "b_num equal" );

  node = maren_rule_set_body( &rule, 0, assemble_start_end, NULL, NULL );
  maren_node_set_dbg_info( node, "start + end" );

  maren_context_add_rule( &ctx, &rule );

  // Rule:: 
  node = maren_rule_add_single_check( &rule, 0,
				      maren_std_single_test_int_eq,
				      &ldc_is_start, NULL );
  maren_node_set_dbg_info( node, "LDC-start" );

  MarenStdSingleTestData ldc_is_intrm = {offsetof( CDR, ldc ),
					 { LDC_INTERMEDIATE }};
  node = maren_rule_add_single_check( &rule, 1,
				      maren_std_single_test_int_eq,
				      &ldc_is_intrm, NULL );
  maren_node_set_dbg_info( node, "LDC-intermediate" );

  node
    = maren_rule_add_double_check( &rule,
				   0, 1,
				   (MarenDoubleCheckFn)cdr_end1_start2_match,
				   NULL, NULL );
  maren_node_set_dbg_info( node, "end-to-start match" );

  node = maren_rule_add_double_check( &rule,
				      0, 1,
				      maren_std_double_test_cstr_eq,
				      &a_num_eq, NULL );
  maren_node_set_dbg_info( node, "a_num equal" );

  node = maren_rule_add_double_check( &rule,
				      0, 1,
				      maren_std_double_test_cstr_eq,
				      &b_num_eq, NULL );
  maren_node_set_dbg_info( node, "b_num equal" );

  node = maren_rule_set_body( &rule, 0, assemble_start_intrm, NULL, NULL );
  maren_node_set_dbg_info( node, "start + intm, recurse" );


  maren_context_add_rule( &ctx, &rule );
  maren_ctx2dot( stdout, &ctx, NULL );

  CDR cdr1 = { "0123", "4321", 0, 10, LDC_FIRST };
  CDR cdr2 = { "0123", "4321", 10, 7, LDC_INTERMEDIATE };
  CDR cdr3 = { "0123", "4321", 17, 4, LDC_LAST };

  maren_context_add_fact( &ctx, &cdr1, (void(*)(void*))del_cdr );
  maren_context_fire( &ctx );

  maren_context_add_fact( &ctx, &cdr3, (void(*)(void*))del_cdr );
  maren_context_fire( &ctx );

  maren_context_add_fact( &ctx, &cdr2, (void(*)(void*))del_cdr );
  maren_context_fire( &ctx );

  maren_context_dtor( &ctx );

  return rc;
}
