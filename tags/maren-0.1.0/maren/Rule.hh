#ifndef MAREN_RULE_HH
#define MAREN_RULE_HH
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

#include "maren.h"
#include "Node.hh"
#include "FunctionObjects.hh"

namespace maren {

  class Context;

  typedef MarenNodePtr NodePtr;

  class Rule {
  public:

    Rule( void ) { maren_dlist_ctor( &rule_ ); }
    ~Rule( void ) { maren_rule_dtor( NULL, &rule_ ); }

    template<typename FactType>
    Node addCheck( unsigned int idx, SingleCheck<FactType>* check );

    template<typename FactType>
    Node addCheck( unsigned int idx, SingleCheck<FactType>& check );

    template<typename LeftFact, typename RightFact>
    Node addCheck( unsigned int leftIdx,
		   unsigned int rightIdx,
		   DoubleCheck<LeftFact, RightFact>* check );

    template<typename LeftFact, typename RightFact>
    Node addCheck( unsigned int leftIdx,
		   unsigned int rightIdx,
		   DoubleCheck<LeftFact, RightFact>& check );

    template<typename ActionType>
    Node setBody( ActionType* action, int priority = 0 );

    template<typename ActionType>
    Node setBody( ActionType& action, int priority = 0 );
    
  private:

    friend class Context;
    MarenDList rule_;

    MarenDList* getDListPtr( void ) { return &rule_; }
  };

#ifdef MAREN_USE_PMF_CONVERSION
  /** \internal This implementation casts a bound pointer to a member
   *  function into a normal function pointer. This might reduce
   *  portability und reqires the option -Wno-pmf-conversions when
   *  using g++. On the other hand it reduces one level of function
   *  calls. */
  template<typename FactType>
  Node
  Rule::addCheck( unsigned int idx, SingleCheck<FactType>* check )
  {
    bool (SingleCheck<FactType>::* pmf)( const FactType* )
      = &SingleCheck<FactType>::operator();

    void(*delFn)(SingleCheck<FactType>*)
      = internal::delete_fn<SingleCheck<FactType> >;

    return maren_rule_add_single_check( &rule_,
					idx,
					(MarenSingleCheckFn)(check->*pmf),
					check,
					(void(*)(void*))delFn );
  }
#else
# error There is currently no version of Rule::addCheck() that works without \
        pmf-conversion. Please try 'configure --enable-pmf-conversion'. 
#endif

#ifdef MAREN_USE_PMF_CONVERSION
  /** \internal This implementation casts a bound pointer to a member
   *  function into a normal function pointer. This might reduce
   *  portability und reqires the option -Wno-pmf-conversions when
   *  using g++. On the other hand it reduces one level of function
   *  calls. */
  template<typename FactType>
  inline Node
  Rule::addCheck( unsigned int idx, SingleCheck<FactType>& check )
  {
    bool(SingleCheck<FactType>::* pmf)( const FactType* )
      = &SingleCheck<FactType>::operator();

    return maren_rule_add_single_check( &rule_,
					idx,
					(MarenSingleCheckFn)(check.*pmf),
					&check,
					NULL );
  }
#else
# error There is currently no version of Rule::addCheck() that works without \
        pmf-conversion. Please try 'configure --enable-pmf-conversion'. 
#endif

#ifdef MAREN_USE_PMF_CONVERSION
  /** \internal This implementation casts a bound pointer to a member
   *  function into a normal function pointer. This might reduce
   *  portability und reqires the option -Wno-pmf-conversions when
   *  using g++. On the other hand it reduces one level of function
   *  calls. */
  template<typename LeftFact, typename RightFact>
  inline Node
  Rule::addCheck( unsigned int leftIdx,
		  unsigned int rightIdx,
		  DoubleCheck<LeftFact, RightFact>* check )
  {
    bool(DoubleCheck<LeftFact,RightFact>::* pmf)( const LeftFact*,
						  const RightFact* )
      = &DoubleCheck<LeftFact, RightFact>::operator();

    void(*delFn)(DoubleCheck<LeftFact,RightFact>*)
      = internal::delete_fn<DoubleCheck<LeftFact, RightFact> >;

    return maren_rule_add_double_check( &rule_,
					leftIdx, rightIdx,
					(MarenDoubleCheckFn)(check->*pmf),
					check,
					(void(*)(void*))delFn );
  }  
#else
# error There is currently no version of Rule::addCheck() that works without \
        pmf-conversion. Please try 'configure --enable-pmf-conversion'. 
#endif

#ifdef MAREN_USE_PMF_CONVERSION
  /** \internal This implementation casts a bound pointer to a member
   *  function into a normal function pointer. This might reduce
   *  portability und reqires the option -Wno-pmf-conversions when
   *  using g++. On the other hand it reduces one level of function
   *  calls. */
  template<typename LeftFact, typename RightFact>
  inline Node
  Rule::addCheck( unsigned int leftIdx,
		  unsigned int rightIdx,
		  DoubleCheck<LeftFact, RightFact>& check )
  {
    bool(DoubleCheck<LeftFact,RightFact>::* pmf)( const LeftFact*,
						  const RightFact* )
      = &DoubleCheck<LeftFact, RightFact>::operator();

    return maren_rule_add_double_check( &rule_,
					leftIdx, rightIdx,
					(MarenDoubleCheckFn)(check.*pmf),
					&check,
					NULL );
  }
#else
# error There is currently no version of Rule::addCheck() that works without \
        pmf-conversion. Please try 'configure --enable-pmf-conversion'. 
#endif

#ifdef MAREN_USE_PMF_CONVERSION
  /** \internal This implementation casts a bound pointer to a member
   *  function into a normal function pointer. This might reduce
   *  portability und reqires the option -Wno-pmf-conversions when
   *  using g++. On the other hand it reduces one level of function
   *  calls. */
  template<typename ActionType>
  inline Node
  Rule::setBody( ActionType* action, int priority )
  {
    void(ActionType::* pmf)(Context&, const ActiveSet&)
      = &ActionType::operator();

    void(*delFn)(ActionType*) = internal::delete_fn<ActionType>;
    
    return maren_rule_set_body( &rule_,
				priority,
				(MarenActionFn)(action->*pmf),
				action,
				(void(*)(void*))delFn );
  }
#else
# error There is currently no version of Rule::setBody() that works without \
        pmf-conversion. Please try 'configure --enable-pmf-conversion'. 
#endif

#ifdef MAREN_USE_PMF_CONVERSION
  /** \internal This implementation casts a bound pointer to a member
   *  function into a normal function pointer. This might reduce
   *  portability und reqires the option -Wno-pmf-conversions when
   *  using g++. On the other hand it reduces one level of function
   *  calls. */
  template<typename ActionType>
  inline Node
  Rule::setBody( ActionType& action, int priority )
  {
    void(ActionType::* pmf)(Context&, const ActiveSet&)
      = &ActionType::operator();
    
    return maren_rule_set_body( &rule_,
				priority,
				(MarenActionFn)(action.*pmf),
				&action,
				NULL );
  }
#else
# error There is currently no version of Rule::setBody() that works without \
        pmf-conversion. Please try 'configure --enable-pmf-conversion'. 
#endif

} // namespace maren
#endif // MAREN_RULE_HH
