#ifndef MAREN_STD_TESTS_H
#define MAREN_STD_TESTS_H
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
#include <stdbool.h>

#include"decls.h"

MAREN_BEGIN_DECL

typedef union {
  int int_v;
  unsigned int u_int_v;
  long long_v;
  char* string_v;
} MarenStdTestArg;

typedef struct {
  size_t offset;
  MarenStdTestArg arg;
} MarenStdSingleTestData;

bool maren_std_single_test_int_eq(  void* data, const void* fact );

typedef struct {
  size_t l_offset;
  size_t r_offset;
} MarenStdDoubleTestData;

bool maren_std_double_test_cstr_eq( void* data,
				    const void* fact1,
				    const void* fact2 );

MAREN_END_DECL
#endif /* MAREN_STD_TESTS_H */
