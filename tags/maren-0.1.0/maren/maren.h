#ifndef MAREN_H
#define MAREN_H
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

/** \file maren.h
 *
 *  \brief The user API include-file.
 *
 *  This is the entry-point for application developers that use the
 *  basic C interface. */

#include <stdarg.h>
 
#include "context.h"
#include "rule.h"
#include "std_tests.h"
#include "dotty.h"
#include "active_set.h"

MAREN_BEGIN_DECL

struct sMarenNode;
typedef struct sMarenNode* MarenNodePtr;

MarenNodePtr maren_rule_get_start( MarenDList* rule, unsigned long idx );
void maren_node_set_dbg_info( struct sMarenNode* node, const char* txt, ... );
void maren_node_set_dbg_info_va( struct sMarenNode* node,
				 const char* txt,
				 va_list ap );

MAREN_END_DECL

#endif /* MAREN_H */
