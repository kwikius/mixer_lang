

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

#include <cstring>
#include <cassert>
#include <limits>

#include "exprtree.hpp"
#include "arg_list.hpp"
#include "function.hpp"
#include "lookup.hpp"
#include "lexer.hpp"
#include "mixer.hpp"
#include "stringfun.hpp"

namespace {

// These need to go in an object!
//###############################
   apm_mix::mixer_t* mixer;
   apm_mix::lookup_t<apm_mix::abc_expr*> * symtab;
   apm_mix::lookup_t<apm_mix::function_builder> * funtab;
//##################################
  
   bool parse_mixer_function();
   apm_mix::abc_expr* parse_expr(); // or_expr
   apm_mix::abc_expr* parse_and_expr();
   apm_mix::abc_expr* parse_rel_expr();
   apm_mix::abc_expr* parse_equality_expr();
   apm_mix::abc_expr* parse_add_expr();
   apm_mix::abc_expr* parse_mul_expr();
   apm_mix::abc_expr* parse_prim_expr();
}

namespace apm_mix{

   bool is_bool_expr(apm_mix::abc_expr* ptr)
   {
      return ptr && (ptr->get_ID() == apm_mix::abc_expr::exprID::BOOL);
   }

   bool are_boolean(apm_mix::abc_expr* lhs, apm_mix::abc_expr* rhs)
   {
      return is_bool_expr(lhs) && is_bool_expr(rhs);
   }

   bool is_float_expr(apm_mix::abc_expr* ptr)
   {
      return ptr && (ptr->get_ID() == apm_mix::abc_expr::exprID::FLOAT);
   }

   bool is_int_expr(apm_mix::abc_expr* ptr)
   {
      return ptr && (ptr->get_ID() == apm_mix::abc_expr::exprID::INT);
   }

   bool is_numeric(apm_mix::abc_expr* ptr)
   {
      return is_int_expr(ptr) || is_float_expr(ptr);
   }

   bool are_numeric(apm_mix::abc_expr* lhs, apm_mix::abc_expr* rhs)
   {
      return is_numeric(lhs) && is_numeric(rhs);
   }

   bool are_same_type(apm_mix::abc_expr* lhs, apm_mix::abc_expr* rhs)
   {
      return  ( lhs && rhs) && (lhs->get_ID() == rhs->get_ID());
   }

   bool are_same_numeric(apm_mix::abc_expr* lhs, apm_mix::abc_expr* rhs)
   {
      return are_same_type(lhs,rhs) && are_numeric(lhs,rhs);
   }
}
   
namespace {
   bool parse_assign_expr(const char*);
   bool parse_output_expr();

   bool  binary_fn_or(bool lhs, bool rhs)
   {
      return lhs || rhs;
   }

   bool  binary_fn_and(bool lhs, bool rhs)
   {
      return lhs && rhs;
   }

   template <typename T>
   bool fun_less(T lhs , T rhs)
   {
       return lhs < rhs;
   }

   template <typename T>
   bool fun_less_equal(T lhs , T rhs)
   {
       return lhs <= rhs;
   }

   template <typename T>
   bool fun_equal_to(T lhs , T rhs)
   {
       return lhs == rhs;
   }

   template <typename T>
   bool fun_not_equal(T lhs , T rhs)
   {
       return lhs != rhs;
   }

   template <typename T>
   bool fun_greater_equal(T lhs , T rhs)
   {
       return lhs >= rhs;
   }

   template <typename T>
   bool fun_greater(T lhs , T rhs)
   {
       return lhs > rhs;
   }

   template <typename T>
   T fun_plus(T lhs , T rhs)
   {
       return lhs + rhs;
   }

   template <typename T>
   T fun_minus(T lhs , T rhs)
   {
       return lhs - rhs;
   }
   template <typename T>
   T fun_mul(T lhs , T rhs)
   {
       return lhs * rhs;
   }

   template <typename T>
   T fun_divide(T lhs , T rhs)
   {
       // avoid division by zero
       if (rhs != T{0}){
          return lhs / rhs; 
       }else{
           return (lhs < T{0})
             ? std::numeric_limits<T>::lowest() // <-- lowest() is correct. The obvious min() does something unexpected on float
             : std::numeric_limits<T>::max();
       }
   }

   template <typename T>
   T fun_negate(T v)
   {
      return -v;
   }

   template <typename T>
   apm_mix::abc_expr * make_rel_op_tpl(int op, apm_mix::expr<T>* lhs, apm_mix::expr<T>* rhs)
   {
      bool (*fun)( T, T) = nullptr;
      switch (op){
         case  apm_lexer::LESS_EQUAL:
            fun = fun_less_equal;
            break;
         case  apm_lexer::GREATER_EQUAL:
            fun = fun_greater_equal;
            break;
         case  '<':
             fun = fun_less;
            break;
         case  '>': 
            fun = fun_greater;
            break;
         default:
            assert(false);
            return nullptr;
      }
      return new apm_mix::binary_op<bool,T>{fun,lhs,rhs};
   }

   template <typename T>
   apm_mix::abc_expr * make_equality_op_tpl(int op, apm_mix::expr<T>* lhs, apm_mix::expr<T>* rhs)
   {
      bool (*fun)(T,T) = nullptr;
      switch (op){
         case  apm_lexer::EQUAL_EQUAL:
            fun = fun_equal_to;
            break;
         case  apm_lexer::NOT_EQUAL:
            fun = fun_not_equal;
            break;
         default:
            assert(false);
            return nullptr;
      }
      return new apm_mix::binary_op<bool,T>{fun,lhs,rhs}; 
   }

   template <typename T>
   apm_mix::abc_expr * make_math_op_tpl(int op, apm_mix::expr<T>* lhs, apm_mix::expr<T>* rhs)
   {
         T (*fun)(T,T) = nullptr;
         switch (op){
            case '+':
               fun = fun_plus;
               break;
            case '-':
               fun = fun_minus;
               break;
            case '*':
               fun = fun_mul;
               break;
            case '/':
               fun = fun_divide;
               break;
            default:
               assert(false);
               return nullptr;
         }
         return new apm_mix::binary_op<T,T>{fun,lhs,rhs}; 
   }

   apm_mix::abc_expr * make_math_op(int op, apm_mix::abc_expr* lhs, apm_mix::abc_expr* rhs)
   {
        if  (is_float_expr(lhs)){
         return make_math_op_tpl(op,(apm_mix::expr<apm_mix::float_t>*) lhs,(apm_mix::expr<apm_mix::float_t>*) rhs);
       }else{
         return make_math_op_tpl(op,(apm_mix::expr<apm_mix::int_t>*) lhs,(apm_mix::expr<apm_mix::int_t>*) rhs);
       }
   }
   // asumes that lhs and rhs are same type
   // and if not float then int
   apm_mix::abc_expr * make_rel_op(int op, apm_mix::abc_expr* lhs, apm_mix::abc_expr* rhs)
   {
       if  (is_float_expr(lhs)){
        // printf("in float rel op\n");
         return make_rel_op_tpl(op,(apm_mix::expr<apm_mix::float_t>*) lhs,(apm_mix::expr<apm_mix::float_t>*) rhs);
       }else{
         // printf("in float rel op\n");
         return make_rel_op_tpl(op,(apm_mix::expr<apm_mix::int_t>*) lhs,(apm_mix::expr<apm_mix::int_t>*) rhs);
       }
   }

   // same types can be float int bool
   apm_mix::abc_expr * make_equality_op(int op, apm_mix::abc_expr* lhs, apm_mix::abc_expr* rhs)
   {
       if  (is_float_expr(lhs)){
        // printf("in float rel op\n");
         return make_equality_op_tpl(op,(apm_mix::expr<apm_mix::float_t>*) lhs,(apm_mix::expr<apm_mix::float_t>*) rhs);
       }else{
           if ( is_int_expr(lhs)){
               return make_equality_op_tpl(op,(apm_mix::expr<apm_mix::int_t>*) lhs,(apm_mix::expr<apm_mix::int_t>*) rhs);
           }else{
                return make_equality_op_tpl(op,(apm_mix::expr<bool>*) lhs,(apm_mix::expr<bool>*) rhs);
           }
       }
   }
}

bool apm_mix::mixer_create(
   const char* filename,
   input_pair* inputs, uint32_t num_inputs,abc_expr** outputs, uint32_t num_outputs)
{

   if (! apm_lexer::open_file(filename)){
      printf("open \"%s\" failed\n",filename);
      return false;
   }
   mixer = new apm_mix::mixer_t{inputs,num_inputs,outputs,num_outputs};
   symtab = new apm_mix::lookup_t<apm_mix::abc_expr*>;
   funtab = new apm_mix::lookup_t<apm_mix::function_builder>;
   funtab->add_item("if",apm_mix::make_function_if);
   funtab->add_item("max",apm_mix::make_function_max);
   funtab->add_item("min",apm_mix::make_function_min);
   for (;;){
      switch(apm_lexer::yylex()){
         case apm_lexer::NAME :
             if ( !parse_assign_expr(apm_lexer::get_lexer_string())){
                apm_lexer::close_file();
                return false;
             }
             break;
         case apm_lexer::MIXER:{
           bool result =  parse_mixer_function();
            apm_lexer::close_file();
            return result;
         }
         default:
            apm_lexer::close_file();
            return apm_mix::yyerror();
      }     
   }
}

void apm_mix::mixer_eval()
{
   mixer->eval_outputs();
}

bool apm_mix::yyerror(const char* str )
{
   printf( "line %i , error : ", apm_lexer::get_line_number());
   if (str){
      printf("%s\n",str);
   }else{
      printf("\n");
   }
   delete mixer; mixer = nullptr;
   delete symtab; symtab = nullptr;
   return false;
}

namespace{

// name '=' Expr ';'
// name is lexer name
   bool parse_assign_expr(const char* name)
   {
      if (apm_lexer::yylex() == '='){
         if ( symtab->find_item(name)){
            return apm_mix::yyerror("name already defined");
         }
         char* name1 = apm_mix::duplicate_string(name);
         apm_mix::abc_expr* expr = parse_expr();
         if ( expr){
            expr = expr->fold();
            if ( apm_lexer::yylex() == ';'){
               symtab->add_item(name1,expr);
               mixer->add(expr);
               return true;
            }else{
               apm_mix::delete_string(name1);
               return apm_mix::yyerror("? ';'");
            }
         }else{
            apm_mix::delete_string(name1);
            return false;
         }
      }else{
         return apm_mix::yyerror("? '='");
      }
   }

/*
 Literal
|   NAME
|  '+' Primary
|  '-' Primary
|  '!' Primary
|   Function
|  IOExpr
|  IF '(' Expr ',' Expr ',' Expr ')'
|  '(' Expr ')'
;
*/
    apm_mix::abc_expr* parse_prim_expr()
    {
      int tok = apm_lexer::yylex();
      switch(tok){
         case apm_lexer::TRUE:
            return new apm_mix::constant<bool>{true};
         case apm_lexer::FALSE:
            return new apm_mix::constant<bool>{false};
         case apm_lexer::INTEGER:
             return new apm_mix::constant<apm_mix::int_t>{apm_lexer::get_lexer_int()};
         case apm_lexer::FLOAT:
             return new apm_mix::constant<apm_mix::float_t>{apm_lexer::get_lexer_float()};
         case apm_lexer::NAME:{
            uint32_t const max_name_len = apm_lexer::get_max_string_chars();
            char name[max_name_len + 1];
            strncpy(name,apm_lexer::get_lexer_string(),max_name_len);
            name[max_name_len] = '\0';
            tok = apm_lexer::yylex();
            if ( tok == '(' ){
            // function_call
               apm_mix::function_builder fn_builder = funtab->find_item(name);
               if( fn_builder != nullptr ) {
                  apm_mix::arg_list * args = nullptr;
                  bool done = false;
                  // all function have at least 1 arg
                  // TODO cap at N args
                  while(! done){
                     auto * arg = parse_expr();
                     if ( arg ){
                        args = add_arg(args,arg);
                        tok = apm_lexer::yylex();
                        switch( tok ){
                           case ',':
                           break;
                           case ')':
                              done = true;
                           break;
                           default:
                              apm_mix::yyerror("function args syntax error");
                              return nullptr;
                        }
                     }else {
                        apm_mix::yyerror("no args");
                        return nullptr;
                     }
                  }
                  if ( done) {
                     return fn_builder(args);
                  }
               }else{
                  char buf[100];
                  snprintf(buf,100,"function \"%s()\" not found",name);
                  apm_mix::yyerror(buf);
                  return nullptr;
               }

            }else{
               apm_lexer::putback(tok);
               auto * result = symtab->find_item(name);
               if (result){
                  return result->clone();
               }else{
                  apm_mix::yyerror("name not found");
                  return nullptr;
               }
            }
         } 
         case apm_lexer::INPUT:{
            if ( apm_lexer::yylex() == '{' ){
               if (apm_lexer::yylex() == apm_lexer::NAME){
                  apm_mix::abc_expr* expr = mixer->find_input(apm_lexer::get_lexer_string());
                     if ( expr){
                        if (apm_lexer::yylex() == '}'){
                           return expr;
                        }else{
                           apm_mix::yyerror("? )");
                        }
                     }else{
                        apm_mix::yyerror("input[name] not found");
                        return nullptr;
                     }
                }else{
                   apm_mix::yyerror("? name");
                   return nullptr;
                }
            }else{
                apm_mix::yyerror("? (");
                return nullptr;
            }
         }
         case '+': // Primary
         case '-':    {    //Primary
             apm_mix::abc_expr* expr = parse_prim_expr();
             if (expr){
                if ( is_numeric(expr) ){
                  if (tok == '-'){
                     if ( is_float_expr(expr) ){
                        return new apm_mix::unary_op<apm_mix::float_t,apm_mix::float_t>{fun_negate,(apm_mix::expr<apm_mix::float_t>*)expr};
                     }else{
                        return new apm_mix::unary_op<apm_mix::int_t, apm_mix::int_t>{fun_negate,(apm_mix::expr<apm_mix::int_t>*)expr};
                     }
                  }else{
                     return expr;
                  }
                }else{
                   apm_mix::yyerror("? + not numeric");
                }
             }else{
               return nullptr;
             }
         }

         case '(': { //  expr ')' 
            auto * result = parse_expr();
            if (result){
               if (apm_lexer::yylex() == ')'){
                     return result;
               }else{
                  apm_mix::yyerror("? )");
                  delete result;
                  return nullptr;
               }
            }else{
               return nullptr;
            }
         }
         default:
             apm_mix::yyerror("? prim");
            return nullptr;
      }
    }

    apm_mix::abc_expr* parse_mul_expr()
    {
       apm_mix::abc_expr* left = parse_prim_expr();
       if (left){
         int tok = apm_lexer::yylex() ;
         for (;;){
            switch (tok){
               case '*':
               case '/':{
                  apm_mix::abc_expr* right = parse_prim_expr();
                  if (right){
                     if ( are_same_numeric(left,right) ){
                         left = make_math_op(tok, left,right);
                         tok = apm_lexer::yylex();
                         break;
                     }else{
                         apm_mix::yyerror("add invalid types");
                         delete left;
                         delete right;
                         return nullptr;
                     }
                  }else{
                     delete left;
                     return nullptr;
                  }
               }
               default:
                  apm_lexer::putback(tok);
                  return left;
             }
          }
       }else{
            return nullptr;
       }
    }

    apm_mix::abc_expr* parse_add_expr()
    {
       apm_mix::abc_expr* left = parse_mul_expr();
       if (left){
         int tok = apm_lexer::yylex() ;
         for (;;){
            switch (tok){
               case '+':
               case '-':{
                  apm_mix::abc_expr* right = parse_mul_expr();
                  if (right){
                     if ( are_same_numeric(left,right) ){
                         left = make_math_op(tok, left,right);
                         tok = apm_lexer::yylex();
                         break;
                     }else{
                         apm_mix::yyerror("add invalid types");
                         delete left;
                         delete right;
                         return nullptr;
                     }
                  }else{
                     delete left;
                     return nullptr;
                  }
               }
               default:
                  apm_lexer::putback(tok);
                  return left;
             }
          }
       }else{
            return nullptr;
       }
    }

    // n.b cant really continue indfinitely
    apm_mix::abc_expr* parse_rel_expr()
    {
        apm_mix::abc_expr* left = parse_add_expr();
        if ( left){
            int tok = apm_lexer::yylex() ;
            for(;;){
               switch( tok){
                  case  apm_lexer::LESS_EQUAL:
                  case  apm_lexer::GREATER_EQUAL:
                  case  '<':
                  case  '>': {
                     apm_mix::abc_expr* right = parse_add_expr();
                     if (right){
                         if ( are_same_numeric(left,right) ){
                            left = make_rel_op(tok, left,right);
                            tok = apm_lexer::yylex();
                            break;
                         }else{
                            apm_mix::yyerror("relop not same type");
                            delete left;
                            delete right;
                            return nullptr;
                         }
                     }else{
                        delete left;
                        return nullptr;
                     }
                  }
                  default:
                     apm_lexer::putback(tok);
                     return left;
               }
            }
        }else{
            return nullptr;
        }
    }

    apm_mix::abc_expr* parse_equality_expr()
    {
       apm_mix::abc_expr* left = parse_rel_expr();
       if ( left){
          int tok = apm_lexer::yylex() ;
          for(;;){
            switch( tok){
               case apm_lexer::EQUAL_EQUAL:
               case apm_lexer::NOT_EQUAL: {
                   apm_mix::abc_expr* right = parse_rel_expr();
                   if (right){
                     if ( are_same_type(left,right)){
                        left = make_equality_op(tok,left,right);
                        tok = apm_lexer::yylex() ;
                        break;
                     }else{
                        apm_mix::yyerror("eqop not same type");
                        delete left;
                        return nullptr;
                     }    
                   }else{
                      delete left;
                      return nullptr;
                   }
               }
               default:
                  apm_lexer::putback(tok);
                  return left;
            }
          }
       }else{
         return nullptr;
       }
      
    }

    apm_mix::abc_expr* parse_and_expr()
    {
       apm_mix::abc_expr* left = parse_equality_expr();
       if ( left){
          int tok = apm_lexer::yylex() ;
          for(;;){
            switch( tok){
               case '&':{
                   apm_mix::abc_expr* right = parse_equality_expr();
                   if ( right){
                     if ( are_boolean(left,right)){
                        left = new apm_mix::binary_op<bool,bool>{
                              binary_fn_and,
                              (apm_mix::expr<bool>*) left, 
                              (apm_mix::expr<bool>*) right
                        };
                        tok = apm_lexer::yylex() ;
                        break;
                     }else{
                        apm_mix::yyerror("bool & bool");
                        delete left;
                        delete right;
                        return nullptr;
                     }    
                   }else{
                      delete left;
                      return nullptr;
                   }
               }
               default:
                  apm_lexer::putback(tok);
                  return left;
            }
          }
       }else{
         return nullptr;
       }
    }

   // or expr
   apm_mix::abc_expr* parse_expr()
   {
       apm_mix::abc_expr* left = parse_and_expr();
       if ( left){
          int tok = apm_lexer::yylex() ;
          for(;;){
            switch( tok){
               case '|':{
                   apm_mix::abc_expr* right = parse_and_expr();
                   if ( right){
                     if ( are_boolean(left,right)){
                        left = new apm_mix::binary_op<bool,bool>{
                              binary_fn_or,
                              (apm_mix::expr<bool>*) left, 
                                (apm_mix::expr<bool>*) right
                        };
                        tok = apm_lexer::yylex() ;
                        break;
                     }else{
                        apm_mix::yyerror("bool | bool");
                        delete left;
                        delete right;
                        return nullptr;
                     }    
                   }else{
                      delete left;
                      return nullptr;
                   }
               }
               default:
                  apm_lexer::putback(tok);
                  return left;
            }
          }
       }else{
         return nullptr;
       }
   }

   //expect  ( expr ) = expr1
   bool parse_output_expr()
   {
      if ( apm_lexer::yylex() == '['){
            apm_mix::abc_expr* expr = parse_expr();
            if ( expr){
              if ( apm_lexer::yylex() == ']'){
                  if ( (expr->get_ID() == apm_mix::abc_expr::exprID::INT) && (expr->is_constant())){
                     auto* expr1 = (apm_mix::expr<apm_mix::int_t>*) expr;
                     uint32_t const idx  = expr1->eval();
                     delete expr; expr = nullptr;
                     
                     if ( apm_lexer::yylex() == '='){
                         apm_mix::abc_expr* expr2 = parse_expr();
                         if( expr2){
                           if (apm_lexer::yylex() == ';'){
                              if ( mixer->add_output(idx,expr2) ){
                                 return true;
                              }else{
                                 delete expr2;
                                 return false;
                              }
                           }else{
                                 delete expr2;
                                 return apm_mix::yyerror("? ;");
                           }
                         }else{
                           return false;
                         }
                     }else{
                       return apm_mix::yyerror("? =");
                     }
                  }else{
                     delete expr;
                     return apm_mix::yyerror("? const int out idx");
                  }
              }else{
                 delete expr;
                 return  apm_mix::yyerror("? ]");
              }
            }else{
               return false;
            }
      }else{
         return apm_mix::yyerror("? [");
      }
   }

   // make the mixer loop function
   //mixer '(' ')' '{' Stmts '}'
   bool parse_mixer_function()
   { 
      int pre[3] = {0,0,0};
      for ( auto & v : pre){
         v = apm_lexer::yylex(); 
      }
      if ( (pre[0] =='(' ) && (pre[1] == ')') && (pre[2] == '{')){
         // TODO
         // at this point evaluate all the current symbols
         // forcing constants e.g whatever the value read is now is the constant value
         for (;;){
            switch(apm_lexer::yylex()){
             case apm_lexer::NAME :
                if ( !parse_assign_expr(apm_lexer::get_lexer_string())){
                  return false;
                }
                break;
            case apm_lexer::OUTPUT :
                if (! parse_output_expr()){
                  return false;
                }
                break;
            case '}': 
         // end of mixer function body done!
         // at this point the symtab can be deleted 
         // maybe only now copy the expressions to the mixer
         // and thereby consolidate memory
               delete symtab;
               symtab = nullptr;
               return true;
            default:
                return apm_mix::yyerror("? input");
            }
         }
      }else{
         return apm_mix::yyerror("? (){");
      }
   }

}//namespace 

