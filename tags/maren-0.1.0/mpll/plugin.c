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
#include "plugin.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dlfcn.h>

static const char path_template[] = "%s/mpll%s.pi";

static bool
complete_plugin( MpllPlugin* plugin )
{
  if ( !(plugin->datas = dlsym( plugin->dl, "mpll_pi_single_checks" )) )
    return false;
  
  if ( !(plugin->single_checks = dlsym( plugin->dl,
					"mpll_pi_single_checks" )) )
    return false;

  size_t count = 0;
  for ( MpllData* it = plugin->datas; it->name; it++ )
    count++;

  qsort( plugin->datas,
	 count,
	 sizeof(MpllData),
	 (int(*)(const void*,const void*))strcmp );

  count = 0;
  for ( MpllSingleCheckPi* it = plugin->single_checks; it->name; it++ )
    count++;

  qsort( plugin->single_checks,
	 count,
	 sizeof(MpllSingleCheckPi),
	 (int(*)(const void*,const void*))strcmp );

  return true;
}

MpllPlugin* mpll_plugin_load( const MpllPluginLoder* loader,
			      const char* plugin )
{
  char filename[ sizeof(path_template) - 4
		 + loader->path_len_max
		 + strlen( plugin ) ];
  void* dl;

  MpllPathNode* path_node;
  maren_dlist_for_each( path, &loader->pi_paths ) {
    path_node = (MpllPathNode*)path;
    sprintf( filename, path_template,
	     path_node->path,
	     plugin );
    
    if ( (dl = dlopen( filename, RTLD_NOW )) ) {
      MpllPlugin* ret = malloc( sizeof(MpllPlugin) );
      MAREN_RT_ASSERT( ret != NULL, "Malloc failed.");
      ret->plugin = plugin;
      ret->dl = dl;
      if ( !complete_plugin( ret ) ) {
	dlclose( dl );
      }
      else
	return ret;
    }
    else {
      fprintf( stderr,
	       "Failed to open plugin:%s:%s.\n",
	       filename,
	       dlerror() );
    }
  }

  return NULL;
}
