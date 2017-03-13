#ifndef MIXER_LANG_LEXER_HPP_INCLUDED
#define MIXER_LANG_LEXER_HPP_INCLUDED

/*
 Copyright (c) 2017 Andy Little 

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>
*/

#include "mixer_lang_types.hpp"

namespace apm_lexer{

   enum tokentype {
    INPUT = 258,
    OUTPUT = 259,
    NAME = 260,
    TRUE = 261,
    FALSE = 262,
    LESS_EQUAL = 263,
    GREATER_EQUAL = 264,
    EQUAL_EQUAL = 265,
    NOT_EQUAL = 266,
    FLOAT = 267,
    INTEGER = 268,
    BOOL = 269,
    VOID = 270,
    MIXER = 271,
    FUN = 272,
    RETURN = 273,
    VOLATILE = 274
  };

   int yylex();
   // n.b this just returns the input buffer
   // You should duplicate the string
   // before calling lex again
   const char* get_lexer_string();
   apm_mix::int_t get_lexer_int();
   apm_mix::float_t get_lexer_float();
   int get_line_number();
   bool putback(int tok);

  // bool open_file( const char * name);
   //void close_file();

   uint32_t get_max_string_chars();

   struct stream_t;

   void set_stream(stream_t * p);
   stream_t * get_stream();
   void close_stream();
}

#endif // MIXER_LANG_LEXER_HPP_INCLUDED
