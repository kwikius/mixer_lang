
#include "function.hpp"
#include "exprtree.hpp"
#include "abc_expr.hpp"
#include "arg_list.hpp"
#include "predicates.hpp"
#include <cstdio>

namespace apm_mix{

   bool yyerror(const char* str );

   // 2 args and both arith
   // then put to outputs
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

// 2 args
apm_mix::abc_expr * apm_mix::make_function_max(apm_mix::arg_list* args)
{
   apm_mix::abc_expr * arg1 = nullptr;
   apm_mix::abc_expr * arg2 = nullptr;
   if (!apm_mix::sigcheck_same_numeric(args,arg1,arg2)){
      return nullptr;
   }

   switch(arg1->get_ID()){
      case abc_expr::exprID::INT:
         return new apm_mix::binary_op<int64_t,int64_t>{fun_max,(apm_mix::expr<int64_t>*)arg1,(apm_mix::expr<int64_t>*)arg2 };
      case abc_expr::exprID::FLOAT:
         return new apm_mix::binary_op<double,double>{fun_max,(apm_mix::expr<double>*)arg1,(apm_mix::expr<double>*)arg2 };
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
         return new apm_mix::binary_op<int64_t,int64_t>{fun_min,(apm_mix::expr<int64_t>*)arg1,(apm_mix::expr<int64_t>*)arg2 };
      case abc_expr::exprID::FLOAT:
         return new apm_mix::binary_op<double,double>{fun_min,(apm_mix::expr<double>*)arg1,(apm_mix::expr<double>*)arg2 };
      default:
         apm_mix::yyerror("unexpected");
         return nullptr;
   }
}
