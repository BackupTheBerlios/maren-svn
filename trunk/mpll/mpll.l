%{
#include <stdio.h>
#include "mpll.tab.h"
%}

%option noyywrap

IDENT [[:alpha:]_][[:alnum:]_]*

%%
[ \t\n]

[#()+,-.:;>?^{}] printf( mplltext );

import return KW_IMPORT;
data return KW_DATA;
unary return KW_UNARY;
binary return KW_BINARY;
as return KW_AS;
always return KW_ALWAYS;

[[:alpha:]_][[:alnum:]_]*
{
   return IDENTIFIER;
}

[+-]?[[:digit:]]+
{
   return INTEGER; 
}

\?{IDENT}
{
   return FACT_REF;
}

\<([^>]|\\>)*> {
   return USER_TEXT;
}
%%

int main( int argc, char *argv[] )
{
   yy_flex_debug = 1;
   while ( yylex() );
}
