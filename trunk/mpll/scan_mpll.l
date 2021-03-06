%{
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
#include <stdio.h>
#include "parse_mpll.h"

int mpllerror( const char* msg )
{
   return 0;
}
%}

%option noyywrap

IDENT [[:alpha:]_][[:alnum:]_]*

%%
[ \t\n]

[#()+,-.;:>?^{}] return *yytext;

import return KW_IMPORT;
data return KW_DATA;
unary return KW_UNARY;
binary return KW_BINARY;
as return KW_AS;
always return KW_ALWAYS;

{IDENT} {
   return IDENTIFIER;
}

[-+]?[[:digit:]]+ {
   return INTEGER;
}

\?{IDENT} {
   return FACT_REF;
}

\<([^>]|\\>)*> {
   return USER_TEXT;
}
%%
