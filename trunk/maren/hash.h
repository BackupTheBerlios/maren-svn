#ifndef MAREN_HASH_H
#define MAREN_HASH_H
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

#include <stddef.h>
#include "decls.h"

MAREN_BEGIN_DECL

typedef struct sMarenHashBucket {
  struct sMarenHashBucket* next;
  unsigned long h;
  const void* key;
  const void* data;
} MarenHashBucket;

#ifdef MAREN_HASH_USE_TRASH_ALLOC
void
maren_hash_reserve_buckets( size_t num );
#endif

typedef enum {
  MAREN_HASH_UNIQUE = (1 << 0),
  MAREN_HASH_FROZEN = (1 << 1)
} MarenHashFlags;

typedef struct sMarenHash {
  unsigned int flags;
  size_t bnum;
  size_t size;
  MarenHashBucket** buckets;
  unsigned long (*hash)( const void* );
  int (*cmp)( const void*, const void* );
} MarenHash;

typedef struct {
  const MarenHash* hash;
  MarenHashBucket** bucket;
  MarenHashBucket* frame;
} MarenHashIter;

#define MAREN_HASH_ITER_INIT { NULL, NULL, NULL }

const void* maren_hash_iter_begin( MarenHashIter* iter,
				   const MarenHash* hash );
#define maren_hash_iter_good(iter) ((iter)->bucket)
#define maren_hash_iter_data(iter) ((iter)->frame->data)
#define maren_hash_iter_key(iter) ((iter)->frame->key)
const void* maren_hash_iter_next( MarenHashIter* iter );
const void* maren_hash_iter_nexteq( MarenHashIter* iter );

MarenHash* maren_hash_create( MarenHash* where,
			  unsigned long (*hash)(const void*),
			  int (*cmp)(const void*, const void*),
			  size_t num_buckets );
			    
void maren_hash_delete( MarenHash* hash, int free_hash );

int maren_hash_key_insert( MarenHash* hash,
			   const void* key,
			   const void* data,
			   MarenHashIter* iter );

void maren_hash_erase( MarenHash* hash, MarenHashIter* iter );

const void* maren_hash_search( MarenHash* hash,
			       const void* key,
			       MarenHashIter* iter );

int maren_hash_optimize( MarenHash* hash );

unsigned long
maren_binary_hash( unsigned long init, const void* key, size_t numByte );

MAREN_END_DECL
#endif /* MAREN_HASH_H */
