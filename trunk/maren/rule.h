#ifndef MAREN_RULE_H
#define MAREN_RULE_H
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

#include "node_functions.h"

/** \file rule.h
 *
 *  \brief Functions to create a single rule.
 *
 *  This file containes all functions, one needs to create a singel
 *  rule. Such a rule, when fully created, can be inserted into a
 *  context. As part of a context, a rule can \e not be modified via
 *  the rule-variable, because the rule will be reset to an empty
 *  list.
 *
 *  There is no specific datatype for a rule to be used. It is
 *  perfecly valid to start with a freshly created \c MarenDList as in
 *  the following example:
 *
 *  \code
 *  MarenDList rule = MAREN_DLIST_INIT;
 *  maren_rule_add_single_check( &rule, ... );
 *  ...
 *  maren_context_add_rule( context_ptr, &rule );
 *  \endcode
 *
 *  \note After adding the rule to the context, the \c rule variable
 *  ist an empty list again. So it is ready to be reused to build a
 *  new rule.
 *
 *  \sa context.h */

MAREN_BEGIN_DECL

struct sMarenDList;
struct sMarenNode;

void maren_rule_dtor( struct sMarenDList* rule );
#define maren_rule_delete( rule ) \
   maren_rule_dtor( rule ); \
   free( rule )

int maren_rule_get_prio( struct sMarenDList* rule );

struct sMarenNode* maren_rule_set_start_hash_hint( struct sMarenDList* rule,
						   unsigned int idx,
						   const void* hint,
						   void(*del_hint)(void*) );

/** \brief Add a new single-check to the rule.
 *
 *  A single check is unary predicate, that can be evaluated with a
 *  fact and some reference data. It can only be evaluated to the two
 *  boolean values \c true or \c false. If the single-check fails, the
 *  fact won't be able to fire a rule in the position \c idx in an
 *  activation set.
 *
 *  \param rule This is the rule, one wants to add the check to.
 *
 *  \param idx This is the index of the fact withing the rule.
 *
 *  \param check This is the check-function, which will be evaluated.
 *
 *  \param data This is some user-defined reference data, that will be
 *  passed to the \a check function for evaluation.
 *
 *  \param del_data In case that \a data was allocated on the heap and
 *  the user doesn't want to handle deallocation himself, one may pass
 *  a delete-function, which releases all resources held for \a data.
 *
 *  \sa MarenSingleCheckFn. */
struct sMarenNode* maren_rule_add_single_check( struct sMarenDList* rule,
						unsigned int idx,
						MarenSingleCheckFn check,
						void* data,
						void(*del_data)(void*) );

struct sMarenNode* maren_rule_add_double_check( struct sMarenDList* rule,
						unsigned int l_idx,
						unsigned int r_idx,
						MarenDoubleCheckFn check,
						void *data,
						void(*del_data)(void*) );

struct sMarenNode* maren_rule_set_body( struct sMarenDList* rule,
					int priority,
					MarenActionFn action,
					void* data,
					void(*del_data)(void*) );

MAREN_END_DECL
#endif /* MAREN_RULE_H */
