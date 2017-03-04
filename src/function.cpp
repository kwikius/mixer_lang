
#include <cstdio>
#include "function.hpp"
#include "exprtree.hpp"
#include "abc_expr.hpp"
#include "arg_list.hpp"
#include "predicates.hpp"
#include "lookup.cpp"

template struct apm_mix::lookup_t<apm_mix::function_builder>;

apm_mix::abc_expr* apm_mix::make_function_if(apm_mix::arg_list* args)
{
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
            return new apm_mix::if_op<apm_mix::float_t>{
               cond
               ,(apm_mix::expr<apm_mix::float_t>*)lhs
               ,(apm_mix::expr<apm_mix::float_t>*)rhs
            };
         case abc_expr::exprID::INT:
            return new apm_mix::if_op<apm_mix::int_t>{
               cond
               ,(apm_mix::expr<apm_mix::int_t>*)lhs
               ,(apm_mix::expr<apm_mix::int_t>*)rhs}
            ;
         case abc_expr::exprID::BOOL:
            return new apm_mix::if_op<bool>{
               cond
               ,(apm_mix::expr<bool>*)lhs
               ,(apm_mix::expr<bool>*)rhs
            };
         default:
            apm_mix::yyerror("unexpected");
            return nullptr;
      }
   }else{
      apm_mix::yyerror("if : invalid types");
      return nullptr;
   }
}

namespace apm_mix{

   bool sigcheck_same_numeric(apm_mix::arg_list* args, abc_expr*& out1, abc_expr* & out2)
   {
      if ( get_num_elements(args) == 2 ){
         auto * arg1 = get_arg(args,0);
         auto * arg2 = get_arg(args,1);
         if (are_same_numeric(arg1,arg2)){
            out1 = arg1;
            out2 = arg2;
            return true;
         }
      }
      return apm_mix::yyerror("invalid args");
   }
}

namespace {

   template <typename T>
   T fun_max(T lhs , T rhs)
   {
      return (lhs > rhs) ? lhs : rhs;
   }
}

apm_mix::abc_expr * apm_mix::make_function_max(apm_mix::arg_list* args)
{
   apm_mix::abc_expr * arg1 = nullptr;
   apm_mix::abc_expr * arg2 = nullptr;
   if (!apm_mix::sigcheck_same_numeric(args,arg1,arg2)){
      return nullptr;
   }

   switch(arg1->get_ID()){
      case abc_expr::exprID::INT:
         return new apm_mix::binary_op<apm_mix::int_t,apm_mix::int_t>{
            fun_max
            ,(apm_mix::expr<apm_mix::int_t>*)arg1
            ,(apm_mix::expr<apm_mix::int_t>*)arg2 
         };
      case abc_expr::exprID::FLOAT:
         return new apm_mix::binary_op<apm_mix::float_t,apm_mix::float_t>{
            fun_max
            ,(apm_mix::expr<apm_mix::float_t>*)arg1
            ,(apm_mix::expr<apm_mix::float_t>*)arg2 
         };
      default:
         apm_mix::yyerror("unexpected");
         return nullptr;
   }
}

namespace {

   template <typename T>
   T fun_min(T lhs , T rhs)
   {
      return (lhs < rhs) ? lhs : rhs;
   }
}

apm_mix::abc_expr * apm_mix::make_function_min(apm_mix::arg_list* args)
{
   apm_mix::abc_expr * arg1 = nullptr;
   apm_mix::abc_expr * arg2 = nullptr;
   if (!apm_mix::sigcheck_same_numeric(args,arg1,arg2)){
      return nullptr;
   }

   switch(arg1->get_ID()){
      case abc_expr::exprID::INT:
         return new apm_mix::binary_op<apm_mix::int_t,apm_mix::int_t>{
            fun_min
            ,(apm_mix::expr<apm_mix::int_t>*)arg1
            ,(apm_mix::expr<apm_mix::int_t>*)arg2 
         };
      case abc_expr::exprID::FLOAT:
         return new apm_mix::binary_op<apm_mix::float_t,apm_mix::float_t>{
            fun_min
            ,(apm_mix::expr<apm_mix::float_t>*)arg1
            ,(apm_mix::expr<apm_mix::float_t>*)arg2 
         };
      default:
         apm_mix::yyerror("unexpected");
         return nullptr;
   }
}
