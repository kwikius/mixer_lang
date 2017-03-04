#include "function.hpp"
#include "exprtree.hpp"
#include "abc_expr.hpp"
#include "arg_list.hpp"
#include "predicates.hpp"
#include <cstdio>

apm_mix::abc_expr * apm_mix::make_function_if(apm_mix::arg_list* args)
{

//signature_check pattern <bool_same_type2(

   if ( get_num_elements(args) != 3 ){
      printf("got %u args\n", get_num_elements(args));
      yyerror("expected 3 args");
      return nullptr;
   }

   auto * abc_cond = get_arg(args,0);
   auto * lhs = get_arg(args,1);
   auto * rhs = get_arg(args,2);

   if (  is_bool_expr(abc_cond) && are_same_type(lhs,rhs) ){
      auto* cond = (apm_mix::expr<bool>*)abc_cond;
      switch(lhs->get_ID()){
         case abc_expr::exprID::FLOAT:
            return new apm_mix::if_op<apm_mix::float_t>{cond,(apm_mix::expr<apm_mix::float_t>*)lhs,(apm_mix::expr<apm_mix::float_t>*)rhs};
         case abc_expr::exprID::INT:
            return new apm_mix::if_op<apm_mix::int_t>{cond,(apm_mix::expr<apm_mix::int_t>*)lhs,(apm_mix::expr<apm_mix::int_t>*)rhs};
         case abc_expr::exprID::BOOL:
            return new apm_mix::if_op<bool>{cond,(apm_mix::expr<bool>*)lhs,(apm_mix::expr<bool>*)rhs};
         default:
            apm_mix::yyerror("unexpected");
            return nullptr;
      }

   }else{
      apm_mix::yyerror("if : invalid types");
      return nullptr;
   }
   
}