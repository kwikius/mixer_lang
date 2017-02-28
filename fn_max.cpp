
#include "fn_max.hpp"

#include "abc_expr.hpp"

namespace apm_mix{

   bool yyerror(const char* str = nullptr);
}

apm_mix::abc_expr * apm_mix::fn_max::make_function(arg_list* args)
{
   // TODO own the args so  delete the args
   yyerror("max todo atam gogogo");
   return nullptr;
}


