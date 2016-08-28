
#include <cctype>
#include <cstring>
#include <cstdio>

#include <quan/conversion/parse_number.hpp>
#include "bison.tab.h"
#include "lexer.hpp"

/*
With lots of inspiration from Bjarne Stroustup's calculator
*/

namespace {
   
   constexpr uint32_t buffer_length = 20;
   char input_buffer[buffer_length];
   uint32_t buffer_idx = 0;
   double  float_value;
   int64_t int_value;
   uint32_t error_num = 0;

   constexpr uint32_t name_too_long = -2;
   constexpr uint32_t number_too_long = -3;
   constexpr uint32_t invalid_number_syntax = -4;
   constexpr uint32_t error_buffer_full = -5;

   constexpr char simplepunct [] = "&|+-*/()[];,{}:";
   constexpr char punct1 [] = "<=>!";
   constexpr int  punct2 [] = {LESS_EQUAL,EQUAL_EQUAL,GREATER_EQUAL,NOT_EQUAL};

   // add line info
   uint32_t line_num = 1;
// crude buffer
// works ok
// unless you put a string or num in there
   bool buffer_full = false;
   int  buffered_tok = 0;
  
    FILE * file_pointer = stdin;
}

bool apm_lexer::open_file( const char * name)
{
   FILE* fp = fopen(name, "r");
   if ( fp != NULL){
      file_pointer = fp;
      return true;
   }else{
      return false;
   }
}

void apm_lexer::close_file()
{
  if ( file_pointer != stdin){
      fclose(file_pointer);
      file_pointer = stdin;
  }
}


int apm_lexer::get_line_number()
{
   return line_num;
}

const char* apm_lexer::get_lexer_string()
{
    return input_buffer;
}

int64_t apm_lexer::get_lexer_int()
{
    return int_value;
}

double apm_lexer::get_lexer_float()
{
    return float_value;
}

int fn_read_char()
{
   return fgetc(file_pointer);
}

int fn_unget_char(int c)
{
   return ungetc(c,file_pointer);
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
       if ( ch == EOF){
         return 0;
       }
       if ( ch == '#'){
         while (ch != '\n'){
             ch = fn_read_char();
             if ( ch == EOF){
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
           // }
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
         return OUTPUT;
      }
      if (strcmp(input_buffer,"if") == 0 ){
         return IF;
      }
      if (strcmp(input_buffer,"input") == 0){
         return INPUT;
      }
      if (strcmp(input_buffer,"mixer") == 0){
         return MIXER;
      }
      if (strcmp(input_buffer,"true") == 0){
         return TRUE;
      }
      if (strcmp(input_buffer,"false") == 0){
         return FALSE;
      }
      if (strcmp(input_buffer,"int") == 0){
         return INTEGER;
      }
      if (strcmp(input_buffer,"float") == 0){
         return FLOAT;
      }
      if (strcmp(input_buffer,"bool") == 0){
         return BOOL;
      }
      if (strcmp(input_buffer,"fun") == 0){
         return FUN;
      }
      if (strcmp(input_buffer,"void") == 0){
         return VOID;
      }
      if (strcmp(input_buffer,"return") == 0){
         return RETURN;
      }
      return NAME;
   }
   // numbers 
   quan::detail::number_parser::num_tok tok = quan::detail::number_parser::get_tok(ch);
   if ( tok != quan::detail::number_parser::num_tok::UNKNOWN){
      input_buffer[0] = ch;
      buffer_idx = 1;
      while (1){
         if ( buffer_idx < (buffer_length -1)){
            ch = fn_read_char();
            if (quan::detail::number_parser::get_tok(ch) != quan::detail::number_parser::num_tok::UNKNOWN) {
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
      quan::detail::number_parser parser;
      quan::detail::number_parser::num_type result = parser(input_buffer,&float_value,&int_value,buffer_idx);
      switch (result){
         case quan::detail::number_parser::num_type::FLOAT:
            return FLOAT;
         case quan::detail::number_parser::num_type::INT:
            return INTEGER;
         default:
            error_num = invalid_number_syntax;
            return 0;
      }
   }
   printf("invalid input");
   return 0;
}







