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
#include "node.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "body_node.h"
#include "start_node.h"
#include "single_node.h"
#include "double_node.h"
#include "std_join_node.h"
#include "dchk_join_node.h"

void maren_node_set_dbg_info_va( MarenNode* node,
				 const char* txt,
				 va_list ap1 )
{
  va_list ap2;
  va_copy( ap2, ap1 );

  if ( node->dbg_info )
    free( node->dbg_info );

  size_t strLen = vsnprintf( NULL, 0, txt, ap1 ) + 1;
  if ( (node->dbg_info = malloc( strLen )) ) {
    vsprintf( node->dbg_info, txt, ap2 );
  }
  va_end( ap2 );
  va_end( ap1 );
}


void
maren_node_set_dbg_info( MarenNode* node, const char* txt, ... )
{
  va_list ap;
  va_start( ap, txt );
  maren_node_set_dbg_info_va( node, txt, ap );
  va_end( ap );
}

void
maren_node_dtor( MarenNode* node )
{
  if ( node->dbg_info )
    free( node->dbg_info );
}

void maren_node_delete( struct sMarenContext* ctx, MarenNode* node )
{
  switch ( maren_node_type( node ) ) {
  case MAREN_NT_BODY:
    maren_body_node_delete( MAREN_BODY(node) );
    break;

  case MAREN_NT_START:
    maren_start_node_delete( MAREN_START(node) );
    break;

  case MAREN_NT_SINGLE:
    maren_single_node_delete( MAREN_SINGLE(node) );
    break;

  case MAREN_NT_DOUBLE:
    maren_double_node_delete( MAREN_DOUBLE(node) );
    break;

  case MAREN_NT_STDJ:
    maren_std_join_node_delete( ctx, MAREN_STDJ(node) );
    break;

  case MAREN_NT_DCHKJ:
    maren_dchk_join_node_delete( ctx, MAREN_DCHKJ(node) );
    break;

  default:
    MAREN_RT_FAIL( "Node with abstract type %d detected.",
		   maren_node_type( node ) );
  }
}



/** \mainpage
 *
 *  \section copying Copying
 *
 *  Copyright (c) 2004 Marcus Perlick \n
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
 * \section intro Introduction
 *
 * So, what is a rule-engine, then?  I heard someone saying, that one
 * should imagine it as a very clever implementation for very long \c
 * if-then-else cascades.  This is a nice hint to get the first idea
 * of the whole thing, but in reality, a rule-engine is a lot more
 * than that.
 *
 * And why is this rule-engine called MAReN?  The first time, I came
 * across rule engines, it was an engine based on the Rete algorithm
 * (todo: give the reference).  And I wondered, why the condition of
 * one rule always is a conjunction of predicates.  I had liked it
 * more, if there could be general boolean expressions about
 * predicates.  So I tried to design a rule system --- inspired by the
 * Rete algorithm --- that lets one give general boolean expressions
 * as a rules condition.  To emphasize, that this system is \e not a
 * Rete implementation, I called it <em>Maren Ain't a Rete
 * Network</em> short <em>MAReN</em>!  Later, I gave up the idea of
 * general boolean expressions, because they would make the whole
 * thing much more complex without extending the expressiveness of the
 * system.  Today, MAReN is based on the Rete algorithm.
 *
 * For the rest of the text I beg you to excuse my lax writing, but
 * there are enough scientific, brilliant, formal writings about rule
 * engines.
 *
 * \subsection rule Rules
 * 
 * To understand, what you can do with this rule-engine, I suppose
 * that you first try to understand, what a rule is.  A rule is
 * something, that consists of exactly two things:
 *
 * -# The rules \e condition.
 * -# The rules \e body.
 *
 * The \e body is something that can be done under certain
 * circumstances.  The \e condition tells us, what circumstance have
 * to be met, to execute the body. An example of a rule could be:
 *
 * \verbatim If there is something that is a house  and
 that house is burning                  and
 there is something that is a phone     and
 that phone is not in the house
 
 then
 
 call the firefighters using that phone.\endverbatim
 *
 * The part above the \c then is the condition and the part below the
 * \c then is the body.  Sounds quite easy, eh?  To make it a bit more
 * complicated, we could write it down in a more formal way:
 *
 * \code
 * (?a, is_house( a ), is_burning( a ), ?b, is_phone( b ), not_in( b, a ))
 * -->
 * ( b.call( firefighters ) )
 * \endcode
 *
 * Ahhh, that looks more like something for real programmers! And what
 * is quite clear from this notation is, that the rule is about two
 * things \f$a\f$ and \f$b\f$. In the future we will call those
 * subjects of a rule \e facts. Then there are some predicates like \c
 * is_house() and so on, that are applied to the facts. As we see,
 * there are unary and binary predicates. The rule will only be
 * executed, if all predicates evaluate to \c true.
 *
 * OK, that's all I have to say about rules in this place. Even, if
 * there are some urgent questions. Some of them might be explained in
 * the next section.
 *
 * \subsection Context
 *
 * Now, that you understood, what a rule is, you might think: "Oh, if
 * I have a rule about two facts, as in the example above, I can give
 * two facts to the rule and it will do something or not."
 *
 * If that would be all about rule-engines, there wouldn't be
 * rule-engines. Testing exactly \f$n\f$ facts against a rule that
 * takes \f$n\f$ facts can be so easily implemented with programming
 * languages like C, C++, Java, C#, Prolog, Haskell or whatever you
 * want, that there is no need to have a special software system for
 * that. No, the secret lies in the <em>"If there is
 * something"</em>-question in the text-version of the rule! The real
 * question is: Where are all those "somethings", that have to be
 * considered? The answer is:
 *
 * They live in the \b context.\n
 *
 * The context is the thing, that defines all our knowledge and
 * wisdom. And not only that, it also knows about all the rules of our
 * little world. This knowledge gives the context a fascinating
 * ability: Given a set of rules, it knows which rules' conditions are
 * fulfilled by which sets of facts. If a rules condition is fulfilled,
 * we say the rule is \e activated.  That means, if the rule \f$R\f$
 * is activated by a set of facts \f$(f_1, f_2, f_3)\f$ and another
 * set \f$(f_{54}, f_{2635}, f_6)\f$ the context knows about that. It
 * knows about all sets that activate a rule and for each of this sets
 * it knows which rule it activates. And not enough with
 * that. Considering that the facts arrived in a sequential order at
 * the context, the context also knows in which order the sets
 * activated their rules.
 *
 * \note Activating a rule does \e not mean to execute it's body!
 *
 * From the practical point of view, the context is a thing that can
 * be equipped with a set of rules. Being done with that, one can feed
 * the context with facts - one after another. And at any time, the
 * context knows which sets of facts activated which rules in which
 * order.
 *
 * This is the point at which we see, that a rule-engine -
 * respectively it's context - is a quite clever thing. But is it
 * useful in some way, also? The answer is: Of course, it can be! And
 * it is useful, because we can not only activate rules, but we also
 * can execute (or \e fire) rules.
 */

/** \page examples Examples
 *
 *  \ref sample_fire_c_basic
 */

/** \page sample_fire_c_basic Example: Fire basic (C interface)
 *  
 *  This is an implementation of the burning-house example rule from
 *  the \ref index page. It only uses the most basic features of the
 *  rule engine. So lets dive into the code:
 *
 *  \dontinclude fire_c_basic.c
 *
 *  The first thing, we have to do, is to include some header
 *  files. The most important for us is the one for the MAReN C API.
 *
 *  \until maren.h
 *
 *  Having this includes, we now have to define some types we can use
 *  to describe our problem. The first one is the House:
 *
 *  \until House
 *
 *  It does not have very much of a house, but having such a house, we
 *  can see if it is burning or not. Then there is one more thing we
 *  need: A phone.
 *
 *  \until Phone
 *
 *  OK, our phone is not a complicated thing, too. But we can see, in
 *  which house the phone is located. This is enough to create our
 *  little world, consisting of houses and phones. To explain this
 *  little world to our rule engine, we need some datatype, that can
 *  keep houses and phones:
 *
 *  \until Fact;
 *
 *  That's all. Now we will implement some of the prediactes, we will
 *  have to use in our example. Note, that we won't write
 *  test-functions for all test, because for some test, we can use
 *  some standard implementations that come with MAReN. But now for
 *  the first test, which will tell us, if a house is burning:
 *
 *  \until }
 *
 *  This function matches the type \c MarenSingleCheckFn, that we
 *  included through the \c maren.h header file. In this case, we
 *  don't use the first parameter, so we give it the name \c
 *  dummy. Because, we declared the parameter \c fact to be
 *  <code>const void*</code> we have to cast it to <code>const
 *  Fact*</code> in the function. The testing code is so simple, that
 *  I don't explain it.
 *
 *  The next test --- or binary predicate --- will tell us, if a fact,
 *  that describes a phone, is in a house described by another fact:
 *
 *  \until }
 *
 *  Note, that the order of the facts in the function's signature is
 *  significant. We will call the first fact the \e left fact and the
 *  second one the \e right fact. We do so, because we could think of
 *  this function as a binary operator, that operates on the both
 *  facts.  We will need this, when we begin to build a rule from our
 *  test-functions. Again, the test-code is so simple, that I don't
 *  explain it. Another noteworthy thing is, that we declated the
 *  facts to be of the type <code>const Fact*</code> which is not
 *  exactly the signature of the \c MarenDoubleCheckFn. On one hand,
 *  passing facts this way lets us implement the test-function without
 *  casting all the time. On the other hand, we have to cats the
 *  function pointer when we build the rule, that uses the function.
 *
 *  \note We allways use const-pointers to facts. We do so, because
 *  the behaviour of the whole rule engine won't be reasonable, if we
 *  would modify facts after they were introduced into a rule-context.
 *
 *  The last thing we need, before we can start building our rule, is
 *  the rule's body, aka the action that calls the firefighters:
 *
 *  \until }

 *  Because, we do not want to write a masterpiece of
 *  reusable software design, we implement everything in the main
 *  function that starts like this:
 *
 *  \until int rc = 0;
 */
