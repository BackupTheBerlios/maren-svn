#ifndef MAREN_FUNCTIONOBJECTS_HH
#define MAREN_FUNCTIONOBJECTS_HH
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

namespace maren {

  class Context;
  class ActiveSet;

  template<typename F>
  struct SingleCheck {
    typedef F FactType;
    virtual ~SingleCheck( void ) {}
    virtual bool operator()( const FactType* fact ) = 0;
  };

  template<typename LF, typename RF = LF>
  struct DoubleCheck {
    typedef LF LeftFactType;
    typedef RF RightFactType;

    virtual ~DoubleCheck( void ) {}
    virtual bool operator()( const LeftFactType* leftFact,
			     const RightFactType* rightFact ) = 0;
  };

  struct Action {
    virtual ~Action( void ) {}
    virtual void operator()( Context& ctx, const ActiveSet& activeSet ) = 0;
  };


  template<typename F, typename M>
  class MemberEq : public SingleCheck<F> {
  public:

    typedef F FactType;
    typedef M MemberType;

    MemberEq( MemberType FactType::* memberPtr,
	      const MemberType& compareValue ) :
      m_( memberPtr ),
      cmp_( compareValue )
    {}

    virtual bool operator()( const FactType* fact  )
    {
      return (fact->*m_) == cmp_;
    }

  private:

    MemberType FactType::* m_;
    MemberType cmp_;
  };

  template<typename F, typename M>
  inline MemberEq<F,M>*
  newMemberEq( M F::* memberPtr, const M& compareValue )
  {
    return new MemberEq<F,M>( memberPtr, compareValue );
  }

  template<typename LF, typename RF, typename M>
  class MembersEq : public DoubleCheck<LF, RF> {
  public:
    
    typedef LF LeftFactType;
    typedef RF RightFactType;
    typedef M MemberType;

    MembersEq( MemberType LeftFactType::* lmPtr,
	       MemberType RightFactType::* rmPtr ) :
      lm_( lmPtr ),
      rm_( rmPtr )
    {} 

    virtual bool operator()( const LeftFactType* lf, const RightFactType* rf )
    {
      return (lf->*lm_) == (rf->*rm_);
    }

  private:
    
    MemberType LeftFactType::* lm_;
    MemberType RightFactType::* rm_;
  };

  template<typename LF, typename RF, typename M>
  inline MembersEq<LF, RF, M>*
  newMembersEq( M LF::* leftMemberPtr, M RF::* rightMemberPtr )
  {
    return new MembersEq<LF, RF, M>( leftMemberPtr, rightMemberPtr );
  }

} // namespace maren
#endif // MAREN_FUNCTIONOBJECTS_HH
