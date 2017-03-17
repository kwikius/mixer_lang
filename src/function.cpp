
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

//#include <cstdio>
#include "error.hpp"
#include "function.hpp"
#include "exprtree.hpp"
#include "abc_expr.hpp"
#include "arg_list.hpp"
#include "predicates.hpp"
#include "lookup.cpp"

template struct apm_mix::lookup_t<apm_mix::function_builder>;

apm_mix::abc_expr* apm_mix::make_function_if(apm_mix::arg_list* args)
{
   int32_t const num_elements = get_num_elements(args);
   if (  num_elements != 3 ){
      yyerror<100>("expected 3 args, got %u",static_cast<unsigned>(num_elements));
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
