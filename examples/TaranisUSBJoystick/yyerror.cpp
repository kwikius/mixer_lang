

#include <mixer_lang.hpp>
#include <cstdio>

bool apm_mix::yyerror(const char* str )
{
   printf( "line %i , error : ", apm_lexer::get_line_number());
   if (str){
      printf("%s\n",str);
   }else{
      printf("\n");
   }
   return false;
}
