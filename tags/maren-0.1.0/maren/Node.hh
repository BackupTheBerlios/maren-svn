#ifndef MAREN_NODE_HH
#define MAREN_NODE_HH
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

#include <stdarg.h>

#include "maren.h"

namespace maren {

  class Rule;

  class Node {
  public:

    Node( void ) : n_( NULL ) {}
    void setDbgInfo( const char* txt, ... );

  private:

    friend class Rule;

    MarenNodePtr n_;

    Node( MarenNodePtr node ) : n_( node ) {}
  };

  inline void
  Node::setDbgInfo( const char* txt, ... )
  {
    if ( n_ ) {
      va_list ap;
      va_start( ap, txt );
      maren_node_set_dbg_info_va( n_, txt, ap );
      va_end( ap );
    }
  }

} // namespace maren;
#endif // MAREN_NODE_HH
