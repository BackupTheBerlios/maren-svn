#ifndef MAREN_CONTEXT_HH
#define MAREN_CONTEXT_HH
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

#include "context.h"
#include "Internal.hh"
#include "Rule.hh"
#include "FactContainer.hh"

namespace maren {

  /** \sa MarenContext */
  class Context : private MarenContext {
  public:

    Context( void ) { maren_context_ctor( this ); }
    ~Context( void ) { maren_context_dtor( this ); }

    void addRule( Rule& rule );

    /** \brief Add a fact without multiple inheritance to the context.
     *
     *  \sa Context::addAs() */
    template<typename FactType>
    int add( const FactType* fact );

    /** \brief Add a fact that somehow untilizes multiple or virtual
     *  inheritance to the context.
     *  
     *  The problem with multiple or virtual inheritance is, that you
     *  equipped the context with some checks that are able to check
     *  facts of certain types. Those checks don't know, if the fact
     *  is really of a type, that is derived from the original
     *  check-type. That's no problem, as long, as there is no need to
     *  adjust a pointer to find the base-class object in the current
     *  fact. Let me give an example:
     *
     *  \code
     *  class Foo { ... };
     *
     *  class CheckAFoo : public maren::SingleCheck<Foo> { ... };
     *
     *  class Bar { ... };
     *  class Baz : public Bar, public Foo { ... };
     *  \endcode
     *
     *  Now you bury a \c CheckAFoo object deeply in a context to
     *  perform tests on \c Foo objects, but you do really add a \c
     *  Baz object to the context. To make sure, that the \c CheckAFoo
     *  gets a pointer to the \c Foo part of the \c Baz you have to
     *  cast the \c Baz to be a \c Foo, before you put it into the
     *  context. You would always have to write
     *
     *  \code
     *  ctx.add( &dynamic_cast<Baz&>( foo ) );
     *  \endcode
     *
     *  But instead of using this obfuscating cast, you could write
     *
     *  \code
     *  ctx.addAs<Baz>( &foo );
     *  \endcode
     *
     *  and the context will take care of doing the dynamic_cast. And
     *  not a least, this form is a little more descriptive to the
     *  reader.
     *
     *  \sa Context::add() */
    template<typename ParamType, typename InternalType>
    int addAs( const ParamType* fact );

    int add( FactContainer& container );
    
    void remove( const void* fact );

    template<typename PType, typename IType>
    void remove( const PType* fact );

    template<typename PType, typename IType>
    PType* release( const PType* fact );

    void fireActivated( void ) { maren_context_fire_activated( this ); }
    void fire( void ) { maren_context_fire( this ); }

    void toDotty( FILE* os ) { maren_ctx2dot( os, this, NULL ); }

  private:

    Context( const Context& );
    Context& operator=( const Context& );
  };

  inline void
  Context::addRule( Rule& rule )
  {
    maren_context_add_rule( this, rule.getDListPtr() );
  }
  
  template<typename FactType>
  inline int
  Context::add( const FactType* fact )
  {
    void(*delFn)(FactType*) = internal::delete_fn<FactType>;
    
    return maren_context_add_fact( this,
				   fact,
				   (void(*)(void*))delFn );
  }

  template<typename ParamType, typename InternalType>
  inline int
  Context::addAs( const ParamType* fact )
  {
    void(*delFn)(InternalType*) = internal::delete_fn<InternalType>;

    return maren_context_add_fact( this,
				   *dynamic_cast<const InternalType&>(*fact),
				   (void(*)(void*))delFn );    
  }

  inline int
  Context::add( FactContainer& container )
  {
    return maren_context_add_factc( this, container.getFactContainerPtr() );
  }

  inline void
  Context::remove( const void* fact )
  {
    maren_context_rm_fact( this, fact );
  }

  template<typename PType, typename IType>
  inline void
  Context::remove( const PType* fact )
  {
    maren_context_rm_fact( this, &dynamic_cast<const IType&>( *fact ) );
  }

  template<typename PType, typename IType>
  inline PType*
  Context::release( const PType* fact )
  {
    this->remove<PType, IType>( fact );
    return const_cast<PType*>( fact );
  }

} // namespace maren
#endif // MAREN_CONTEXT_HH
