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
#include <string>
#include <iostream>

#include <maren/Maren.hh>

enum LDCState {
  LDC_COMPLETE,
  LDC_FIRST,
  LDC_INTERMEDIATE,
  LDC_LAST
};

struct CDR;

std::ostream&
operator<<( std::ostream& os, const CDR& cdr );

struct CDR {
  std::string a_num;
  std::string b_num;
  long start;
  unsigned int duration;
  LDCState ldc;

  CDR( const std::string& aNum,
       const std::string& bNum,
       long st,
       unsigned int dur,
       LDCState l ) :
    a_num( aNum ),
    b_num( bNum ),
    start( st ),
    duration( dur ),
    ldc( l )
  {}

  ~CDR( void )
  {
    std::cout << (*this) << std::endl;
  }
};

std::ostream&
operator<<( std::ostream& os, const CDR& cdr )
{
  os << cdr.a_num << ' ' << cdr.b_num << ' ' << cdr.start << ' '
     << cdr.duration << ' ' << cdr.ldc;
  return os;
}

struct End1Start2Match : public maren::DoubleCheck<CDR>
{
  virtual bool operator()( const CDR* left, const CDR* right )
  {
    long leftEnd = left->start + left->duration;
    long d = right->start - leftEnd;
    return d >= 0 && d < 2;
  }
};

struct AssembleStartEnd : public maren::Action
{
  virtual void operator()( maren::Context& ctx,
			   const maren::ActiveSet& activeSet )
  {
    const CDR* start = activeSet.getFactAs<CDR>( 0 );
    const CDR* end = activeSet.getFactAs<CDR>( 1 );
    
    std::cout << '(' << (*start) << ") + (" << (*end) << ") = (";

    ctx.remove( start );
    ctx.remove( end );

    CDR* complete = const_cast<CDR*>( start );
    complete->duration += end->duration;
    complete->ldc = LDC_COMPLETE;

    std::cout << (*complete) << ") !" << std::endl;
  }
};

struct AssembleStartIntrm : public maren::Action
{
  virtual void operator()( maren::Context& ctx,
			   const maren::ActiveSet& activeSet )
  {
    const CDR* start = activeSet.getFactAs<CDR>( 0 );
    const CDR* end = activeSet.getFactAs<CDR>( 1 );
    
    std::cout << '(' << (*start) << ") + (" << (*end) << ") = (";

    CDR* newStart = ctx.release<CDR, CDR>( start );
    ctx.remove( end );

    newStart->duration += end->duration;

    std::cout << (*newStart) << ") ->" << std::endl;

    ctx.add( activeSet.getContainer( 0 ) );
  }
};

int main( int argc, char *argv[] )
{
  int rc = 0;
  maren::Context ctx;
  maren::Rule rule;
  maren::Node node;

  node = rule.addCheck( 0, maren::newMemberEq( &CDR::ldc, LDC_FIRST ) );
  node.setDbgInfo( "LDC-start" );

  node = rule.addCheck( 1, maren::newMemberEq( &CDR::ldc, LDC_LAST ) );
  node.setDbgInfo( "LDC-last" );

  End1Start2Match end1Start2Match;
  node = rule.addCheck( 0, 1, end1Start2Match );
  node.setDbgInfo( "end-to-start match" );
  
  node = rule.addCheck(0, 1, maren::newMembersEq( &CDR::a_num, &CDR::a_num ));
  node.setDbgInfo( "a_num equal" );

  node = rule.addCheck(0, 1, maren::newMembersEq( &CDR::b_num, &CDR::b_num ));
  node.setDbgInfo( "b_num equal" );

  AssembleStartEnd assmStartEnd;
  node = rule.setBody( assmStartEnd );

  ctx.addRule( rule );

  
  node = rule.addCheck( 0, maren::newMemberEq( &CDR::ldc, LDC_FIRST ) );
  node.setDbgInfo( "LDC-start" );

  node = rule.addCheck( 1, maren::newMemberEq( &CDR::ldc, LDC_INTERMEDIATE ) );
  node.setDbgInfo( "LDC-intermediate" );

  node = rule.addCheck( 0, 1, end1Start2Match );
  node.setDbgInfo( "end-to-start match" );

  node = rule.addCheck(0, 1, maren::newMembersEq( &CDR::a_num, &CDR::a_num ));
  node.setDbgInfo( "a_num equal" );

  node = rule.addCheck(0, 1, maren::newMembersEq( &CDR::b_num, &CDR::b_num ));
  node.setDbgInfo( "b_num equal" );

  AssembleStartIntrm assmStartIntrm;
  node = rule.setBody( assmStartIntrm );

  ctx.addRule( rule );
  ctx.toDotty( stdout );

  ctx.add( new CDR( "0123", "4321", 0, 10, LDC_FIRST ) );
  ctx.fire();

  ctx.add( new CDR( "0123", "4321", 10, 7, LDC_INTERMEDIATE ) );
  ctx.fire();

  ctx.add( new CDR( "0123", "4321", 17, 4, LDC_LAST ) );
  ctx.fire();

  return rc;
}
