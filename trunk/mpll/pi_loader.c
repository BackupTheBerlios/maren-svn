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
#include "pi_loader.h"

#include <stdlib.h>
#include <string.h>

#include "pi_loader.h"

MpllPluginLoder*
mpll_plugin_loader_ctor( void* where )
{
  MpllPluginLoder* ret = where ? where : malloc( sizeof(MpllPluginLoder) );

  if ( ret ) {
    maren_dlist_ctor( &ret->pi_paths );
    ret->path_len_max = 0;
  }

  return ret;
}

void
mpll_plugin_loader_add_path( MpllPluginLoder* loader, const char* path )
{
  maren_dlist_for_each( node, &loader->pi_paths ) {
    if ( !strcmp( ((MpllPathNode*)node)->path, path ) ) {
      return;
    }
  }

  MpllPathNode* node = calloc( 1, sizeof(MpllPathNode) );

  MAREN_RT_ASSERT( node != NULL, _("Malloc error.") );

  node->path = path;
  maren_dlist_append( &loader->pi_paths, (MarenDList*)node );

  size_t len = strlen( path );
  if ( len > loader->path_len_max )
    loader->path_len_max = len;
}
