#ifndef MAREN_MPLL_PI_LOADER_H
#define MAREN_MPLL_PI_LOADER_H
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

#include <maren/dlist.h>
#include <maren/hash.h>

MAREN_BEGIN_DECL

typedef struct {
  MarenDList list;
  const char* path;
} MpllPathNode;

typedef struct {
  MarenDList pi_paths;
  size_t path_len_max;
  MarenHash pi_hash;
} MpllPluginLoder;

MpllPluginLoder* mpll_plugin_loader_ctor( void* where );

void mpll_plugin_loader_add_path( MpllPluginLoder* loader, const char* path );

MAREN_END_DECL
#endif /* MAREN_MPLL_PI_LOADER_H */
