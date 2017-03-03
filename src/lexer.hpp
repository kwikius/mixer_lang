#ifndef MIXER_LANG_LEXER_HPP_INCLUDED
#define MIXER_LANG_LEXER_HPP_INCLUDED

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
   const char* get_lexer_string();
   int64_t get_lexer_int();
   double get_lexer_float();
   int get_line_number();
   bool putback(int tok);

   bool open_file( const char * name);
   void close_file();

   uint32_t get_max_string_chars();
}

#endif // MIXER_LANG_LEXER_HPP_INCLUDED
