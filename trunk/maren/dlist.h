#ifndef MAREN_DLIST_H
#define MAREN_DLIST_H
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

typedef struct sMarenDList {
  struct sMarenDList *p, *n;
} MarenDList;

#define MAREN_DLIST_INIT { NULL, NULL }

#define maren_dlist_ctor( dlist_ptr ) \
   ((dlist_ptr)->p = (dlist_ptr)->n = NULL)

void maren_dlist_append( MarenDList* l, MarenDList* n );
void maren_dlist_join( MarenDList* first, MarenDList* second );

#define maren_dlist_empty( dlPtr ) (((MarenDList*)(dlPtr))->p == NULL)
#define maren_dlist_first( dlPtr ) (((MarenDList*)(dlPtr))->p)
#define maren_dlist_last( dlPtr ) (((MarenDList*)(dlPtr))->n)
#define maren_dlist_iter_next( dlIter ) ((dlIter)->n)
#define maren_dlist_iter_get( dlIter ) ((void*)(((MarenDList*)(dlIter)) + 1))

/** \internal Remove the node \a n from lst \a l.
 *
 *  This function modifiles the list \a l so that the node \a n is no
 *  longer an element of \a l. The node \a n is modified, so that it
 *  is not linked to any other node.
 *
 *  \note If the node \a n is \e not an element of \a l, the behaviour
 *  of the function is undefined.
 *
 *  \param l This is a pointer to the doubly linked list.
 *
 *  \param n This is a pointer to a node of the list, which has to be
 *  removed.
 *
 *  \return Returns the succesor of the deleted node or \c NULL. */
MarenDList* maren_dlist_iter_rm( MarenDList* l, MarenDList* n );

#define maren_dlist_for_each( iter, list ) \
   for ( MarenDList* iter = maren_dlist_first( list ); \
         iter; \
         iter = maren_dlist_iter_next( iter ) )

#define maren_dlist_iter_next( dlIter ) ((dlIter)->n)

/** \internal This is offset-get (-> \c _oget), which gets one the node,
 *  if the \c MarenDList element is \e not the first one in the node's
 *  data structure. E.g. the following code will run perfectly:
 * 
 *  \code
 *  typedef struct {
 *     int some_data;
 *     MarenDList list_link;
 *  } FooType;
 *
 *  int main( int argc, char *argv[] ) {
 *     FooType foo;
 *     MarenDList* node = &foo.list_link;
 *     assert( maren_dlist_iter_oget( node, FooType, list_link ) == &foo );
 *     return 0;
 *  }
 *  \endcode */
#define maren_dlist_iter_oget( iter, Type, listNode ) \
   ((Type*)((char*)(iter) - offsetof( Type, listNode )))

MAREN_END_DECL
#endif /* MAREN_DLIST_H */
