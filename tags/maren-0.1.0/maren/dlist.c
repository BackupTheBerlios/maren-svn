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
#include "dlist.h"

#include <stddef.h>
#include <assert.h>

void
maren_dlist_append( MarenDList* l, MarenDList* n )
{
  if ( l->p == NULL ) {
    assert( l->n == NULL );

    l->p = l->n = n;
  }
  else {
    assert( l->n != NULL );

    n->p = l->n;
    n->n = NULL;
    l->n->n = n;
    l->n = n;
  }
}

void
maren_dlist_join( MarenDList* first, MarenDList* second )
{
  if ( first->p == NULL ) {
    assert( first->n == NULL );
    *first = *second;
    return;
  }
  
  if ( second->p == NULL ) {
    assert( second->n == NULL );
    *second = *first;
    return;
  }

  first->n->n = second->p;
  second->p->p = first->n;

  first->n = second->n;
  second->p = first->p;
}

MarenDList*
maren_dlist_iter_rm( MarenDList* l, MarenDList* n )
{
  /* Remove the 1st element in the list */
  if ( l->p == n ) {
    if ( (l->p = n->n) ) {
      l->p->p = NULL;
    }
    else {
      l->n = NULL;
    }
    n->p = n->n = NULL;
    return l->p;		/* EARLY RETURN */
  }

  /* List lenght is > 1! Otherwise, n woulde have been the 1st in the
   * list. */

  /* Remove the last element in the list of length > 1. */
  if ( l->n == n ) {
    l->n = n->p;
    l->n->n = NULL;
    n->p = n->n = NULL;
    return NULL;		/* EARLY RETURN */
  }

  /* Remove some intermediate node in a list with at least 3
   * elements. */
  n->p->n = n->n;
  n->n->p = n->p;
  n->p = n->n = NULL;
  return n->n;
}
