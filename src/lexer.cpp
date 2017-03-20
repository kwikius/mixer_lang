
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

#include <cctype>
#include <cstring>
//#include <cstdio>

#include "parse_number.hpp"
#include "lexer.hpp"
#include "error.hpp"
#include "lexer_stream.hpp"

/*
With lots of inspiration from Bjarne Stroustup's calculator
*/

namespace {
   
   // iow max string chars = 49;
   constexpr uint32_t buffer_length = 50;
   char input_buffer[buffer_length];
   uint32_t buffer_idx = 0;
   apm_mix::float_t  float_value;
   apm_mix::int_t int_value;
   uint32_t error_num = 0;

   constexpr uint32_t name_too_long = -2;
   constexpr uint32_t number_too_long = -3;
   constexpr uint32_t invalid_number_syntax = -4;
   constexpr uint32_t error_buffer_full = -5;

   constexpr char simplepunct [] = "&|+-*/()[];,{}:";
   constexpr char punct1 [] = "<=>!";
   constexpr int  punct2 [] = {
      apm_lexer::LESS_EQUAL
      ,apm_lexer::EQUAL_EQUAL
      ,apm_lexer::GREATER_EQUAL
      ,apm_lexer::NOT_EQUAL
   };

   // add line info
   uint32_t line_num = 1;
// crude buffer
// works ok
// unless you put a string or num in there! :(
   bool buffer_full = false;
   int  buffered_tok = 0;
  
  // FILE * file_pointer = stdin;
   apm_lexer::stream_t * p_stream = nullptr;
}

uint32_t apm_lexer::get_max_string_chars()
{
    return buffer_length - 1;
}

// reset everything
void apm_lexer::set_stream(apm_lexer::stream_t * p)
{
   p_stream = p;
   line_num = 1;
   buffer_full = false;
   buffered_tok = 0;
   error_num = 0;
   buffer_idx = 0;
}

apm_lexer::stream_t* apm_lexer::get_stream()
{
   return p_stream;
}

void apm_lexer::close_stream()
{
  p_stream = nullptr;
}

int apm_lexer::get_line_number()
{
   return line_num;
}

const char* apm_lexer::get_lexer_string()
{
    return input_buffer;
}

apm_mix::int_t apm_lexer::get_lexer_int()
{
    return int_value;
}

apm_mix::float_t apm_lexer::get_lexer_float()
{
    return float_value;
}

int fn_read_char()
{
   if ( p_stream != nullptr){
      return p_stream->get_char();
   }else{
      return -1;
   }
}

int fn_unget_char(int c)
{
   if (p_stream != nullptr){
      return p_stream->unget_char(c);
   }else{
      return -1;
   }
}

int fn_peek_char()
{
   int ch = fn_read_char();
   fn_unget_char(ch);
   return ch;
}

bool apm_lexer::putback(int tok)
{
   if (!buffer_full){
      buffered_tok = tok;
      buffer_full = true;
      return true;
   }else{
      error_num = error_buffer_full;
      return false;
   }
}

using apm_mix::util::number_parser;
// add checks for eof
int apm_lexer::yylex()
{
   if ( buffer_full){
       buffer_full = false;
       return buffered_tok;
   }
   int ch ;
   do{
       ch = fn_read_char();
       if ( ch == -1){
         return 0;
       }
       if ( ch == '#'){
         while (ch != '\n'){
             ch = fn_read_char();
             if ( ch == -1){
               return 0;
             }
         }
      }
      if ( ch == '\n'){
         ++line_num;
      }
   } while (std::isspace(ch) );
 
   for ( auto p : simplepunct){
      if (ch == p){
         return ch;
      }
   }
   int idx = 0;
   for ( auto p : punct1){
      if (ch == p){
         char ch1 = fn_read_char();
         if ( ch1 == '='){
            return punct2[idx];
         }else{
            fn_unget_char(ch1);
            return ch;
         }
      }else{
         ++idx;
      }
   }
   // NAME and reserved names
   if ( isalpha(ch)  || (ch == '_') ){
      buffer_idx = 0;
      do{
         if ( buffer_idx < (buffer_length-1)){
           input_buffer[buffer_idx] = ch;
           ++buffer_idx;
         }else{
            input_buffer[buffer_idx] ='\0';
            error_num = name_too_long;
            fn_unget_char(ch);
            return 0; //fail
         }
         ch = fn_read_char();
      }while( std::isalnum(ch) || (ch == '_') );
      // 
      fn_unget_char(ch);

      // look for reserved names
      input_buffer[buffer_idx] ='\0';
      if (strcmp(input_buffer,"output") == 0 ){
         return apm_lexer::OUTPUT;
      }

      if (strcmp(input_buffer,"input") == 0){
         return apm_lexer::INPUT;
      }
      if (strcmp(input_buffer,"mixer") == 0){
         return apm_lexer::MIXER;
      }
      if (strcmp(input_buffer,"true") == 0){
         return apm_lexer::TRUE;
      }
      if (strcmp(input_buffer,"false") == 0){
         return apm_lexer::FALSE;
      }
      if (strcmp(input_buffer,"int") == 0){
         return apm_lexer::INTEGER;
      }
      if (strcmp(input_buffer,"float") == 0){
         return apm_lexer::FLOAT;
      }
      if (strcmp(input_buffer,"bool") == 0){
         return apm_lexer::BOOL;
      }
      if (strcmp(input_buffer,"fun") == 0){
         return apm_lexer::FUN;
      }
      if (strcmp(input_buffer,"void") == 0){
         return apm_lexer::VOID;
      }
      if (strcmp(input_buffer,"return") == 0){
         return apm_lexer::RETURN;
      }
      return apm_lexer::NAME;
   }
   // numbers 
   number_parser::num_tok tok = number_parser::get_tok(ch);
   if ( tok != number_parser::num_tok::UNKNOWN){
      input_buffer[0] = ch;
      buffer_idx = 1;
      while (1){
         if ( buffer_idx < (buffer_length -1)){
            ch = fn_read_char();
            if (number_parser::get_tok(ch) != number_parser::num_tok::UNKNOWN) {
               input_buffer[buffer_idx] = ch;
               ++buffer_idx;
            }else{
               fn_unget_char(ch);
               input_buffer[buffer_idx] = '\0';
               ++buffer_idx;
               break;
            }
         }else{
            error_num = number_too_long;
            return 0;
         }
      }
      number_parser parser;
      number_parser::num_type result = parser(input_buffer,&float_value,&int_value,buffer_idx);
      switch (result){
         case number_parser::num_type::FLOAT:
            return apm_lexer::FLOAT;
         case number_parser::num_type::INT:
            return apm_lexer::INTEGER;
         default:
            error_num = invalid_number_syntax;
            return 0;
      }
   }
   apm_mix::yyerror("invalid input");
   return 0;
}
