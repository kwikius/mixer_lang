#include <cstdio>
#include <cstring>
#include <malloc.h>
#include "bison.tab.h"
#include "exprtree.hpp"
#include "symtab.hpp"
#include "lexer.hpp"
#include "mixer.hpp"
#include <cassert>

namespace {
   apm_mix::mixer_t* mixer;
   apm_mix::symtab_t * symtab;
   bool do_mix_loop();
   apm_mix::abc_expr* do_expr(); // or_expr
   apm_mix::abc_expr* do_and_expr();
   apm_mix::abc_expr* do_rel_expr();
   apm_mix::abc_expr* do_add_expr();
   apm_mix::abc_expr* do_mul_expr();
   apm_mix::abc_expr* do_prim_expr();

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
   
   bool do_assign_expr(const char*);
   
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
   apm_mix::abc_expr * make_rel_op_tpl(int op, apm_mix::expr<T>* lhs, apm_mix::expr<T>* rhs)
   {
        printf("in make t\n");
         bool (*fun)( T, T) = nullptr;
         switch (op){
                  case  LESS_EQUAL:
                     fun = fun_less_equal;
                     break;
                  case  GREATER_EQUAL:
                     fun = fun_greater_equal;
                     break;
                  case  EQUAL_EQUAL:
                     fun = fun_equal_to;
                     break;
                  case  NOT_EQUAL:
                     fun = fun_not_equal;
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

   // asumes that lhs and rhs are same type
   // and if not float then int
   apm_mix::abc_expr * make_rel_op(int op, apm_mix::abc_expr* lhs, apm_mix::abc_expr* rhs)
   {
       if  (is_float_expr(lhs)){
         printf("in float rel op\n");
         return make_rel_op_tpl(op,(apm_mix::expr<double>*) lhs,(apm_mix::expr<double>*) rhs);
       }else{
          printf("in float rel op\n");
         return make_rel_op_tpl(op,(apm_mix::expr<int64_t>*) lhs,(apm_mix::expr<int64_t>*) rhs);
       }
   }
}

bool fn_mix()
{
   mixer = new apm_mix::mixer_t;
   symtab = new apm_mix::symtab_t;
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
            return apm_mix::yyerror("name n//a");
         }
         char* name1 = strdup(name);
         apm_mix::abc_expr* expr = do_expr();
         if ( expr){
            if ( apm_lexer::yylex() == ';'){
               symtab->add_item(name1,expr);
               mixer->add(expr);
               printf("got assign\n");
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
            auto * result = symtab->find_item(apm_lexer::get_lexer_string());
            if (result){
               return result->m_node->clone();
            }else{
               apm_mix::yyerror("name not found");
               return nullptr;
            }
         }   
         default:
            return nullptr;
      }
    }

    apm_mix::abc_expr* do_mul_expr()
    {
       return do_prim_expr();
    }


    apm_mix::abc_expr* do_add_expr()
    {
       return do_mul_expr();
    }


    apm_mix::abc_expr* do_rel_expr()
    {
        printf("in rel expr\n");
        apm_mix::abc_expr* left = do_add_expr();
        if ( left){
            int tok = apm_lexer::yylex() ;
            for(;;){
               switch( tok){
                  case  LESS_EQUAL:
                  case  GREATER_EQUAL:
                  case  EQUAL_EQUAL:
                  case  NOT_EQUAL:
                  case  '<':
                  case  '>': {
                     apm_mix::abc_expr* right = do_add_expr();
                     if (right){
                         if ( are_same_numeric(left,right) ){
                            left = make_rel_op(tok, left,right);
                            tok = apm_lexer::yylex();
                            break;
                         }else{
                            apm_mix::yyerror("binop : lhs rhs op not same type");
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

    apm_mix::abc_expr* do_and_expr()
    {
       apm_mix::abc_expr* left = do_rel_expr();
       if ( left){
          int tok = apm_lexer::yylex() ;
          for(;;){
            switch( tok){
               case '&':{
                   apm_mix::abc_expr* right = do_rel_expr();
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

   //expect  [ expr ] = expr1
   bool do_output_expr()
   {
      return false;
   }

   // make the mixer loop function
   //mixer '{' Stmts '}'
   bool do_mix_loop()
   { 
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
         default:
             return apm_mix::yyerror("? input");
         }
      }
   }

}//namespace 

