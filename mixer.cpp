#include <cstdio>
#include <cstring>
#include <malloc.h>
#include "bison.tab.h"
#include "exprtree.hpp"
#include "arg_list.hpp"
#include "function.hpp"
#include "lookup.hpp"
#include "lexer.hpp"
#include "mixer.hpp"
#include <cassert>
#include <quan/meta/max.hpp>
#include <quan/meta/min.hpp>

namespace {
   apm_mix::mixer_t* mixer;
   apm_mix::lookup_t<apm_mix::abc_expr*> * symtab;
   apm_mix::lookup_t<apm_mix::function_builder> * funtab;
  
   bool do_mix_loop();
   apm_mix::abc_expr* do_expr(); // or_expr
   apm_mix::abc_expr* do_and_expr();
   apm_mix::abc_expr* do_rel_expr();
   apm_mix::abc_expr* do_equality_expr();
   apm_mix::abc_expr* do_add_expr();
   apm_mix::abc_expr* do_mul_expr();
   apm_mix::abc_expr* do_prim_expr();
  // apm_mix::abc_expr* do_if_expr();
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
   bool do_assign_expr(const char*);
 //  bool do_function_def();
   
   bool do_output_expr();

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
              ? quan::meta::min_<T>::value
              : quan::meta::max_<T>::value;
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
         case  LESS_EQUAL:
            fun = fun_less_equal;
            break;
         case  GREATER_EQUAL:
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
         case  EQUAL_EQUAL:
            fun = fun_equal_to;
            break;
         case  NOT_EQUAL:
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
         return make_math_op_tpl(op,(apm_mix::expr<double>*) lhs,(apm_mix::expr<double>*) rhs);
       }else{
         return make_math_op_tpl(op,(apm_mix::expr<int64_t>*) lhs,(apm_mix::expr<int64_t>*) rhs);
       }
   }
   // asumes that lhs and rhs are same type
   // and if not float then int
   apm_mix::abc_expr * make_rel_op(int op, apm_mix::abc_expr* lhs, apm_mix::abc_expr* rhs)
   {
       if  (is_float_expr(lhs)){
        // printf("in float rel op\n");
         return make_rel_op_tpl(op,(apm_mix::expr<double>*) lhs,(apm_mix::expr<double>*) rhs);
       }else{
         // printf("in float rel op\n");
         return make_rel_op_tpl(op,(apm_mix::expr<int64_t>*) lhs,(apm_mix::expr<int64_t>*) rhs);
       }
   }

   // same types can be float int bool
   apm_mix::abc_expr * make_equality_op(int op, apm_mix::abc_expr* lhs, apm_mix::abc_expr* rhs)
   {
       if  (is_float_expr(lhs)){
        // printf("in float rel op\n");
         return make_equality_op_tpl(op,(apm_mix::expr<double>*) lhs,(apm_mix::expr<double>*) rhs);
       }else{
           if ( is_int_expr(lhs)){
               return make_equality_op_tpl(op,(apm_mix::expr<int64_t>*) lhs,(apm_mix::expr<int64_t>*) rhs);
           }else{
                return make_equality_op_tpl(op,(apm_mix::expr<bool>*) lhs,(apm_mix::expr<bool>*) rhs);
           }
       }
   }
}

void apm_mix::mixer_init(input_pair* inputs, uint32_t num_inputs)
{
    mixer = new apm_mix::mixer_t{inputs,num_inputs};
    symtab = new apm_mix::lookup_t<apm_mix::abc_expr*>;
    funtab = new apm_mix::lookup_t<apm_mix::function_builder>;
    funtab->add_item("if",apm_mix::make_function_if);
    funtab->add_item("max",apm_mix::make_function_max);
    funtab->add_item("min",apm_mix::make_function_min);
}

void apm_mix::eval_mixer_outputs()
{
   mixer->eval_outputs();
}

bool fn_mix()
{
   for (;;){
      switch(apm_lexer::yylex()){
         case NAME :
             if ( !do_assign_expr(apm_lexer::get_lexer_string())){
               return false;
             }
             break;
         case MIXER:
           return do_mix_loop();
         default:
            return apm_mix::yyerror();
      }     
   }
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
   bool do_assign_expr(const char* name)
   {
      if (apm_lexer::yylex() == '='){
         if ( symtab->find_item(name)){
            return apm_mix::yyerror("name already defined");
         }
         char* name1 = strdup(name);
         apm_mix::abc_expr* expr = do_expr();
         if ( expr){
            expr = expr->fold();
            if ( apm_lexer::yylex() == ';'){
               symtab->add_item(name1,expr);
               mixer->add(expr);
              // printf("got assign\n");
               return true;
            }else{
               return apm_mix::yyerror("? ';'");
            }
         }else{
           // printf("assign fail\n");
            free(name1);
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
    apm_mix::abc_expr* do_prim_expr()
    {
      int tok = apm_lexer::yylex();
      switch(tok){
         case TRUE:
            return new apm_mix::constant<bool>{true};
         case FALSE:
            return new apm_mix::constant<bool>{false};
         case INTEGER:
             return new apm_mix::constant<int64_t>{apm_lexer::get_lexer_int()};
         case FLOAT:
             return new apm_mix::constant<double>{apm_lexer::get_lexer_float()};
         case NAME:{
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
                     auto * arg = do_expr();
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
         case INPUT:{
            if ( apm_lexer::yylex() == '{' ){
               if (apm_lexer::yylex() == NAME){
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
             apm_mix::abc_expr* expr = do_prim_expr();
             if (expr){
                if ( is_numeric(expr) ){
                  if (tok == '-'){
                     if ( is_float_expr(expr) ){
                        return new apm_mix::unary_op<double,double>{fun_negate,(apm_mix::expr<double>*)expr};
                     }else{
                        return new apm_mix::unary_op<int64_t, int64_t>{fun_negate,(apm_mix::expr<int64_t>*)expr};
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
         // case '!': Primary
//         case IF : //( expr, Expr, Expr)
//            return do_if_expr();
         case '(': { //  expr ')' 
            auto * result = do_expr();
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

    apm_mix::abc_expr* do_mul_expr()
    {
       apm_mix::abc_expr* left = do_prim_expr();
       if (left){
         int tok = apm_lexer::yylex() ;
         for (;;){
            switch (tok){
               case '*':
               case '/':{
                  apm_mix::abc_expr* right = do_prim_expr();
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

    apm_mix::abc_expr* do_add_expr()
    {
       apm_mix::abc_expr* left = do_mul_expr();
       if (left){
         int tok = apm_lexer::yylex() ;
         for (;;){
            switch (tok){
               case '+':
               case '-':{
                  apm_mix::abc_expr* right = do_mul_expr();
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
    apm_mix::abc_expr* do_rel_expr()
    {
        apm_mix::abc_expr* left = do_add_expr();
        if ( left){
            int tok = apm_lexer::yylex() ;
            for(;;){
               switch( tok){
                  case  LESS_EQUAL:
                  case  GREATER_EQUAL:
                  case  '<':
                  case  '>': {
                     apm_mix::abc_expr* right = do_add_expr();
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

    apm_mix::abc_expr* do_equality_expr()
    {
       apm_mix::abc_expr* left = do_rel_expr();
       if ( left){
          int tok = apm_lexer::yylex() ;
          for(;;){
            switch( tok){
               case EQUAL_EQUAL:
               case NOT_EQUAL: {
                   apm_mix::abc_expr* right = do_rel_expr();
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

    apm_mix::abc_expr* do_and_expr()
    {
       apm_mix::abc_expr* left = do_equality_expr();
       if ( left){
          int tok = apm_lexer::yylex() ;
          for(;;){
            switch( tok){
               case '&':{
                   apm_mix::abc_expr* right = do_equality_expr();
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
   apm_mix::abc_expr* do_expr()
   {
       apm_mix::abc_expr* left = do_and_expr();
       if ( left){
          int tok = apm_lexer::yylex() ;
          for(;;){
            switch( tok){
               case '|':{
                   apm_mix::abc_expr* right = do_and_expr();
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
   bool do_output_expr()
   {
      if ( apm_lexer::yylex() == '['){
            apm_mix::abc_expr* expr = do_expr();
            if ( expr){
              if ( apm_lexer::yylex() == ']'){
                  if ( (expr->get_ID() == apm_mix::abc_expr::exprID::INT) && (expr->is_constant())){
                     auto* expr1 = (apm_mix::expr<int64_t>*) expr;
                     uint32_t const idx  = expr1->eval();
                     delete expr; expr = nullptr;
                     
                     if ( apm_lexer::yylex() == '='){
                         apm_mix::abc_expr* expr2 = do_expr();
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
   bool do_mix_loop()
   { 
      int pre[3] = {0,0,0};
      for ( auto & v : pre){
         v = apm_lexer::yylex(); 
      }
      if ( (pre[0] =='(' ) && (pre[1] == ')') && (pre[2] == '{')){
         for (;;){
            switch(apm_lexer::yylex()){
             case NAME :
                if ( !do_assign_expr(apm_lexer::get_lexer_string())){
                  return false;
                }
                break;
            case OUTPUT :
                if (! do_output_expr()){
                  return false;
                }
                break;
            case '}':
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

