#include "function.hpp"
#include "exprtree.hpp"
#include "abc_expr.hpp"
#include "arg_list.hpp"
#include "predicates.hpp"
#include <cstdio>

apm_mix::abc_expr * apm_mix::make_function_if(apm_mix::arg_list* args)
{
   if ( get_num_elements(args) != 3 ){
      printf("got %u args\n", get_num_elements(args));
      yyerror("expected 3 args");
      return nullptr;
   }
   auto * cond = get_arg(args,0);
   auto * lhs = get_arg(args,1);
   auto * rhs = get_arg(args,2);

   if (  is_bool_expr(cond) && are_same_type(lhs,rhs) ){
      switch(lhs->get_ID()){
         case abc_expr::exprID::FLOAT:
            return new apm_mix::if_op<double>{(apm_mix::expr<bool>*)cond,(apm_mix::expr<double>*)lhs,(apm_mix::expr<double>*)rhs};
         case abc_expr::exprID::INT:
            return new apm_mix::if_op<int64_t>{(apm_mix::expr<bool>*)cond,(apm_mix::expr<int64_t>*)lhs,(apm_mix::expr<int64_t>*)rhs};
         case abc_expr::exprID::BOOL:
            return new apm_mix::if_op<bool>{(apm_mix::expr<bool>*)cond,(apm_mix::expr<bool>*)lhs,(apm_mix::expr<bool>*)rhs};
         default:
            apm_mix::yyerror("unexpected");
            return nullptr;
      }

   }else{
      apm_mix::yyerror("if : invalid types");
      return nullptr;
   }
   
}