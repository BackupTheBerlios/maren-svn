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
#include "hash.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* Found it somewhere on the net! It's not my idea. It seems to make
 * good distributions of elements into the has-buckets. Anyone an
 * idea, who invented this thing? */ 
unsigned long
maren_binary_hash( unsigned long init, const char* key, size_t numByte )
{
  while ( numByte > 0 ) {
    init ^= (init << 7) ^ (*key) ^ (init >> 3);
    if ( --numByte == 0 )
      break;
    key++;
    init ^= ~((init << 11) ^ (*key) ^ (init >> 5));
    numByte--, key++;
  }

  return init;
}

#ifdef MAREN_HASH_USE_TRASH_ALLOC
/** \todo The Bucket allocator is not yet MT safe. So, do something to
 *  give the user a chance to use the hash in an TM environment. */
static MarenHashBucket* the_buckets = NULL;

void
maren_hash_reserve_buckets( size_t num )
{
  MarenHashBucket* nb;

  while ( num > 0 ) {
    nb = malloc( sizeof(MarenHashBucket) );
    nb->next = the_buckets;
    the_buckets = nb;

    num--;
  }
}

static MarenHashBucket*
new_bucket( void )
{
  MarenHashBucket* ret;

  if ( the_buckets ) {
    ret = the_buckets;
    the_buckets = ret->next;
  }
  else {
    ret = malloc( sizeof(MarenHashBucket) );
  }

  return ret;
}
#else
static MarenHashBucket*
new_bucket( void )
{
  return malloc( sizeof(MarenHashBucket) );
}
#endif

const void*
maren_hash_iter_begin( MarenHashIter* iter, const MarenHash* hash )
{
  iter->hash = hash;
  iter->bucket = hash->buckets;
  if ( !(iter->frame == *(iter->bucket)) ) {
    MarenHashBucket** bEnd = hash->buckets + hash->bnum;
    do {
      if ( ++(iter->bucket) >= bEnd ) {
	assert( iter->bucket == bEnd );
	iter->bucket = NULL;
	return NULL;
      }
    } while ( !*(iter->bucket) );
      
    iter->frame = *(iter->bucket);
  }

  assert( iter->frame != NULL );
  assert( iter->frame->data != NULL );

  return iter->frame->data;
}


const void*
maren_hash_iter_next( MarenHashIter* iter )
{
  if ( !(iter->bucket) ) {
    assert( iter->frame == NULL );
    return NULL;
  }

  assert( iter->frame != NULL );

  if ( !(iter->frame = iter->frame->next) ) {
    MarenHashBucket** bEnd = iter->hash->buckets + iter->hash->bnum;
    do {
      if ( ++(iter->bucket) >= bEnd ) {
	assert( iter->bucket == bEnd );
	iter->bucket = NULL;
	return NULL;
      }
    } while ( !*(iter->bucket) );
      
    iter->frame = *(iter->bucket);
  }

  assert( iter->frame != NULL );
  assert( iter->frame->data != NULL );

  return iter->frame->data;
}

const void*
maren_hash_iter_nexteq( MarenHashIter* iter )
{
  const void* data;

  if ( !iter->frame )
    return NULL;
  
  data = iter->frame->data;
  while ( (iter->frame = iter->frame->next) ) {
    if ( !iter->hash->cmp( data, iter->frame->data ) )
      return iter->frame->data;
  }

  iter->bucket = NULL;
  return NULL;
}

#ifndef MAREN_INIT_BUCKET_NUM
# define MAREN_INIT_BUCKET_NUM 8999
#endif

MarenHash*
maren_hash_create( MarenHash* where,
		  unsigned long (*hash)(const void*),
		  int (*cmp)(const void*, const void*),
		  size_t num_buckets )
{
  MarenHash* ret = where ? where : malloc( sizeof(MarenHash) );

  if ( ret ) {
    ret->bnum = num_buckets > 0 ? num_buckets : MAREN_INIT_BUCKET_NUM;
    if ( (ret->buckets = calloc( ret->bnum, sizeof(MarenHashBucket*) )) ) {
      ret->flags = 0;
      ret->size = 0;
      ret->hash = hash;
      ret->cmp = cmp;
    }
    else {
      free( ret );
      ret = NULL;
    }
  }
  
  return ret;
}

#ifdef MAREN_HASH_USE_TRASH_ALLOC
static void
del_bucket( MarenHashBucket* bucket )
{
  assert( bucket != NULL );

  bucket->next = the_buckets;
  the_buckets = bucket;
}
#else
static void
del_bucket( MarenHashBucket* bucket )
{
  free( bucket );
}
#endif

static void
maren_hash_clear_bucket( MarenHashBucket* bucket )
{
  register MarenHashBucket* tmp;
  
  while ( bucket ) {
    tmp = bucket->next;
    del_bucket( bucket );
    bucket = tmp;
  }
}

void
maren_hash_delete( MarenHash* hash, int free_hash )
{
  MarenHashBucket **bEnd, **bIt;

  for ( bIt = hash->buckets, bEnd = bIt + hash->bnum; bIt < bEnd; bIt++ ) {
    if ( *bIt )
      maren_hash_clear_bucket( *bIt );
  }

  free( hash->buckets );
  
  if ( free_hash )
    free( hash );
}

int
maren_hash_key_insert( MarenHash* hash,
		       const void* key,
		       const void* data,
		       MarenHashIter* iter )
{
  assert( key != NULL );

  unsigned long h = hash->hash( key );
  MarenHashBucket** bucket = hash->buckets + (h % hash->bnum);
  MarenHashBucket* frame;

  if ( hash->flags & MAREN_HASH_UNIQUE ) {
    MarenHashBucket* b = *bucket;
    while ( b ) {
      if ( !hash->cmp( b->key, key ) ) {
	if ( iter ) {
	  iter->hash = hash;
	  iter->bucket = bucket;
	  iter->frame = b;
	}
	return 1;
      }
    }
  }

  frame = new_bucket();

  assert( frame != NULL );

  frame->next = *bucket;
  frame->h = h;
  frame->key = key;
  frame->data = data;

  *bucket = frame;

  hash->size++;

  if ( iter ) {
    iter->hash = hash;
    iter->bucket = bucket;
    iter->frame = frame;
  }

  return 0;
}

const void* maren_hash_search( MarenHash* hash,
			      const void* key,
			      MarenHashIter* iter )
{
  const void* ret = NULL;
  unsigned long h = hash->hash( key );
  MarenHashBucket** bucket = hash->buckets + (h % hash->bnum);
  MarenHashBucket* frame =  *bucket;

  while ( frame ) {
    if ( !hash->cmp( frame->key, key ) ) {
      ret = frame->data;
      if ( iter ) {
	iter->hash = hash;
	iter->bucket = bucket;
	iter->frame = frame;
      }
    }
    frame = frame->next;
  }

  return ret;
}

//static size_t
//maren_hash_best_bucket_num( const MarenHash* hash, size_t from, size_t to )
//{
//  MarenHashBucket **b, **e, *f;
//  size_t max, opt, optMax = hash->size;
//  size_t *s, *se;
//  size_t *stat = malloc( to * sizeof(size_t) );
//
//  while ( from <= to ) {
//    memset( stat, 0, to * sizeof(size_t) );
//    
//    for ( b = hash->buckets, e = b + hash->bnum; e < b; e++ ) {
//      for ( f = *b; f; f = f->next ) {
//	stat[ f->h & from ]++;
//      }
//    }
//
//    max = 0;
//    for ( s = stat, se = s + from; s < se; s++ ) {
//      if ( *s > max )
//	max = *s;
//    }
//
//    if ( max < optMax ) {
//      opt = from;
//      optMax = max;
//    }
//
//    from++;
//  }
//
//  return opt;
//}

int
maren_hash_optimize( MarenHash* hash )
{
  MarenHashBucket **new_buckets, **nb;
  MarenHashBucket **b, **e, *f, *tf;
  size_t tbn = (10 * hash->size) / 8; /* Rule of thumb: at most 80% filled. */

  // shrink hash only if it is significantly oversized
  if ( tbn < hash->bnum && tbn > (hash->bnum / 2) )
    return 1;

  if ( !(new_buckets = calloc( tbn, sizeof(MarenHashBucket*) )) )
    return 2;

  for ( b = hash->buckets, e = b + hash->bnum; e < b; e++ ) {
    for ( f = *b; f; f = tf ) {
      tf = f->next;

      nb = new_buckets + (f->h % tbn);
      f->next = *nb;
      *nb = f;
    }
  }

  return 0;
}
