#ifndef MAREN_DECLS_H
#define MAREN_DECLS_H
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

#include <assert.h>

#ifdef __cplusplus
# define MAREN_BEGIN_DECL extern "C" {
# define MAREN_END_DECL }
#else
# define MAREN_BEGIN_DECL
# define MAREN_END_DECL
#endif

MAREN_BEGIN_DECL

/** \internal Use this to test conditions, which do \e not depend on
 *  the code to be correct, but on some runtime-characteristics such
 *  as user-prvided parameters, ect. */
//#define MAREN_RT_ASSERT( cond, ... ) assert( cond )
//#define MAREN_RT_FAIL( ... ) assert( 0 );
#define MAREN_RT_ASSERT( cond, ... ) \
   if ( !(cond) ) { \
      maren_error_fn( __FILE__, __LINE__, __func__, #cond , __VA_ARGS__ ); \
      assert( cond ); \
   }

#define MAREN_RT_FAIL( ... ) \
   maren_error_fn( __FILE__, __LINE__, __func__, "FAILED", __VA_ARGS__ ); \
   assert( 0 )

extern void (*maren_error_fn)( const char* file,
			       int line,
			       const char* func,
			       const char* cond,
			       const char* desc, ... );

MAREN_END_DECL
#endif /* MAREN_DECLS_H */
