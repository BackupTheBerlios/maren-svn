#include <stdio.h>

#include <maren/dlist.h>

typedef struct {
  MarenDList list;
  int i;
} Node1;

int main( int argc, char *argv[] )
{
  int rc = 0;
  MarenDList list = MAREN_DLIST_INIT;

  Node1 n1, n2, n3;

  maren_dlist_append( &list, (MarenDList*)&n1 );
  maren_dlist_append( &list, (MarenDList*)&n2 );
  maren_dlist_append( &list, (MarenDList*)&n3 );

  maren_dlist_for_each( it, &list ) {
    printf( "%d\n", ((Node1*)it)->i );
  }

  return rc;
}
