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
  int mplllex( void );
  int mpllerror( const char* );
%}

%name-prefix="mpll"
%error-verbose

%union {
  char* str_v;
  int int_v;
}

%token KW_IMPORT
%token KW_DATA KW_UNARY KW_BINARY KW_AS KW_ALWAYS
%token <str_v> IDENTIFIER FACT_REF USER_TEXT
%token <int_v> INTEGER

%%
context
   : rule_section
   | import_section rule_section
   ;

import_section
   : import
   | import_section import
   ;

import
   : KW_IMPORT block_import
   | KW_IMPORT import_subject import_list ';'
   ;

block_import: IDENTIFIER '{' block_import_section_list '}'
   ;

block_import_section_list
   : block_import_section
   | block_import_section_list block_import_section
   ;

block_import_section
   : import_subject ':' uq_import_list
   ;

uq_import_list
   : uq_import
   | uq_import_list ',' uq_import
   ;

uq_import
   : IDENTIFIER
   | IDENTIFIER KW_AS IDENTIFIER
   ;

import_list
   : single_import
   | import_list ',' single_import
   ;

single_import
   : q_name
   | q_name KW_AS IDENTIFIER
   ;

import_subject
   : KW_DATA
   | KW_UNARY
   | KW_BINARY
   ;

rule_section
   : rule_sec_element
   | rule_section rule_sec_element
   ;

rule_sec_element
   : prio_decl rule_definition
   | prio_decl prio_block
   ;

prio_block
   : '{' rule_def_list '}'
   ;

rule_def_list
   :
   | rule_def_list rule_definition
   ;

prio_decl
   : '.' opt_prio_delta ':'
   | '#' opt_prio_delta ':'
   | '^' opt_prio_delta ':'
   | INTEGER ':'
   ;

opt_prio_delta
   :
   | '-' INTEGER
   | '+' INTEGER
   ;

rule_definition
   : IDENTIFIER rule_signature '?' condition '>' action '.'
   ;

rule_signature
   : '(' identifier_list ')'
   ;

condition
   : KW_ALWAYS
   | test_list
   ;

test_list
   : predicate
   | test_list ',' predicate
   ;

predicate
   : IDENTIFIER '(' parameter_list ')'
   | q_name '(' parameter_list ')'
   ;

parameter_list
   : FACT_REF
   | USER_TEXT
   | parameter_list ',' FACT_REF
   | parameter_list ',' USER_TEXT
   ; 

action
   :
   ;

identifier_list
   :
   | IDENTIFIER
   | identifier_list ',' IDENTIFIER
   ;

q_name: IDENTIFIER ':' IDENTIFIER
      ;
%%
