#ifndef MIXER_LANG_LEXER_HPP_INCLUDED
#define MIXER_LANG_LEXER_HPP_INCLUDED

namespace apm_lexer{

   int yylex();
   const char* get_lexer_string();
   int64_t get_lexer_int();
   double get_lexer_float();
   int get_line_number();
   bool putback(int tok);
}

#endif // MIXER_LANG_LEXER_HPP_INCLUDED
