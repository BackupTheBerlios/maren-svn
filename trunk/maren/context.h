#ifndef MAREN_CONTEXT_H
#define MAREN_CONTEXT_H
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
#include <config.h>

#include <stdlib.h>
#include <stdbool.h>

#include "dlist.h"

#if MAREN_ACTIVE_SET_ALLOC != 0
#include "as_alloc.h"
#endif

MAREN_BEGIN_DECL

/** \file context.h
 *
 * \todo Modifying a stored fact ist a questionable thing! Can it be
 * prevented? */

struct sMarenHash;
struct sMarenFactContainer;
struct sMarenActiveSet;
struct sMarenNode;

typedef struct {
  int prio;
  struct sMarenActiveSet *first_active, *last_active;
} MarenActivePriority;

typedef struct sMarenContext {
  MarenDList start_nodes;
  size_t prio_num;
  MarenActivePriority* prios;
  MarenActivePriority* tmpp;	/**< Only, 'caus fire's not MT-safe, anyway */
  struct sMarenHash* start_hash;
  const void* (*fact_key_hash)(const void*,void(**del_key)(void*));
#if MAREN_ACTIVE_SET_ALLOC != 0
  MarenActiveSetAlloc as_alloc;
#endif
} MarenContext;

#define MAREN_CONTEXT_INIT { { NULL, NULL }, 0, NULL, NULL, NULL }

MarenContext* maren_context_ctor( void* where );
void maren_context_dtor( MarenContext* ctx );
#define maren_context_delete( ctx ) \
   (maren_context_dtor( ctx ), \
    free( ctx ))

void maren_context_add_rule( MarenContext* ctx, MarenDList* rule );

int maren_context_add_fact( MarenContext* ctx,
			    const void* fact,
			    void (*del_fact)(void*) );

int maren_context_add_factc( MarenContext* ctx,
			     struct sMarenFactContainer* fact_cntr );

void maren_context_rm_fact( MarenContext* ctx, const void* fact );

void maren_context_fire_activated( MarenContext* ctx );

bool maren_context_has_active( MarenContext* ctx );

#define maren_context_fire( ctx ) \
   while ( maren_context_has_active( ctx ) ) { \
      maren_context_fire_activated( ctx ); \
   }

#define maren_context_has_start_hash( ctx ) ((ctx)->start_hash != NULL)

/** \brief Activat the start-hash feature for a given context.
 *  
 *  In general, a new fact has to enter the rete-network at each
 *  start-node.  But in practice, it is often possible to subdivied
 *  the whole domain of all facts into several classes with each class
 *  having its own distinct set of start-nodes in the rete-network. In
 *  such a case, it would be enough, to pass the new fact only to
 *  those start-nodes, that are in the class that matches the
 *  fact. Doing so could speed up adding new facts to a context
 *  significantly.
 *
 *  Theoreticaly, each subdivision of a domain into distinct classes
 *  can be formulated as a hash. So we use a hash in the context, to
 *  implement the optimisation, mentioned above.
 *
 *  \param ctx This is the context, for which we want to use the
 *  start-hash feature.
 *
 *  \param fact_key The first thing we have to do, when using the
 *  start-hash feature, is to extract the hash-key from the fact. This
 *  will be done by the function passed with this pointer for each new
 *  fact. The problem is, that only the user knows, if the function
 *  only returns a pointer, that point into the facts memory, making
 *  it unnecessary to free the hash-key when it is no longer
 *  needed. In more complex cases, the function might allocate memory
 *  on the heap, which implies that there have to be some means to
 *  cleanly delete the hash-key. To solve that problem, the \c
 *  fact_key function get the fact as the 1st parameter and a pointe
 *  to a function-pointer, that has to be initialized by \c fact_key,
 *  while computing the key. This way \c fact_key can return a
 *  delete-function to the context, if the hash-key has to be
 *  deleted. The return value of \c fact_key has to be a pointer to
 *  the hash-key.
 *
 *  \param hash_fn <todo>
 *  \param hash_cmp <todo>
 *
 *  \param start_buckes This is the number of initially allocated
 *  buckes in the hash. If you pass a\c 0 here, a reasonable number
 *  will be used.*/
void maren_context_set_start_hash( MarenContext* ctx,
				   const void*(*fact_key)(const void*,
							  void(**)(void*)),
				   unsigned long (*hash_fn)(const void*),
				   int (*hash_cmp)(const void*, const void*),
				   size_t start_buckets );

typedef struct sMarenContextVisitor {
  void (*start_context)( void* data, const MarenContext* v );
  void* ctx_start_data;

  void (*end_context)( void* data, const MarenContext* v );
  void* ctx_end_data;

  void (*node_fn)( void* data, const struct sMarenNode* node );
  void* node_fn_data;
} MarenContextVisitor;

void maren_context_visit( MarenContext* ctx, MarenContextVisitor* visitor );

MAREN_END_DECL
#endif /* MAREN_CONTEXT_H */
