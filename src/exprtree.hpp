#ifndef MIXER_LANG_EXPRTREE_HPP_INCLUDED
#define MIXER_LANG_EXPRTREE_HPP_INCLUDED

#include <cstdint>
#include "mixer_lang_types.hpp"
#include "abc_expr.hpp"

namespace apm_mix{

   bool yyerror(const char* str = nullptr);

   struct mixer_t;

   template <typename T>
   struct get_type_id;

   template<>
   struct get_type_id<apm_mix::int_t>{
      static constexpr auto value = abc_expr::exprID::INT;
   };

   template<>
   struct get_type_id<apm_mix::float_t>{
      static constexpr auto value = abc_expr::exprID::FLOAT;
   };

   template<>
   struct get_type_id<bool>{
      static constexpr auto value = abc_expr::exprID::BOOL;
   };
      
   template <typename T>
   struct expr: abc_expr{
      virtual T eval() const =0;
      expr():abc_expr{get_type_id<T>::value}{}
      expr & operator = (expr const &) = delete;
      expr (expr const &) = delete;
   };

   template <typename T>
   struct constant : expr<T>{
      constant(T value_in):m_value{value_in}{}
      T eval()const {return m_value;}
      bool is_constant() const { return true;}
      expr<T>* fold() { return this;}
      expr<T>* clone() const { return new constant{this->m_value};}
      
   private:
      T m_value;
   };

   

   template <typename T>
   struct input : expr<T>{
      typedef T ( *pfn_get) ();
      T eval()const {return m_pfn_get();}
      bool is_constant() const {return false;}
      expr<T>* fold() { return this;}
      expr<T>* clone() const { return new input{this->m_pfn_get};}
      input(pfn_get get_fn_in)
      : m_pfn_get{get_fn_in}{}
      
      private:
         pfn_get m_pfn_get;
   };

   template <typename T>
   struct output : expr<T> {

      typedef void (*pfn_send)( T const &);
      output(pfn_send put_fn_in):m_pfn_send{put_fn_in}, m_expr{nullptr}{}
      T eval()const 
      { 
         if ( m_expr) { 
            return m_expr->eval();
         }else {
            return T{0} ;
         }
      }
      bool has_output_expr() const { return m_expr != nullptr;}
      bool set_output_expr(expr<T> * e) 
      {
         if (m_expr == nullptr){
            m_expr = e; 
            return true;
         }else{ 
            return false;
         }
      }
      bool is_constant() const { return false;}
      expr<T>* fold() { if ( m_expr) {m_expr = (expr<T>*)m_expr->fold();} return this;}
      expr<T>* clone() const { return new output{this->m_pfn_send, m_expr};}
      void apply() { m_pfn_send(this->eval());}
      private:
      // called by clone
      output(pfn_send put_fn_in, expr<T> * e)
      :m_pfn_send{put_fn_in}, m_expr{e}{}
      pfn_send m_pfn_send;
      expr<T> * m_expr;
   };

   template <typename Result , typename Arg>
   struct unary_op : expr<Result>{
      typedef Result (*unary_fun)( Arg);
      unary_op(unary_fun fun_in,expr<Arg> * arg_in)
      :m_fun{fun_in}, m_arg{(expr<Arg>*)arg_in->fold()}{}
      ~unary_op()
      { 
         if (m_arg != nullptr){delete m_arg;}
      }
      bool is_constant() const { return m_arg->is_constant();}
      expr<Result>* fold()
      {
         if ( m_arg->is_constant() ){
             auto* result = new constant<Result>(this->eval());
             delete this;
             return result;
         }else{
            return this;
         }
      }
      Result eval() const 
      {
         return m_fun(m_arg->eval());
      }
      expr<Result>* clone() const  
      {
         return new unary_op{this->m_fun,(expr<Arg>*)this->m_arg->clone()}; 
      }
   private:
      unary_fun  m_fun;
      expr<Arg> * m_arg;
   };

   template <typename Result , typename Arg>
   struct binary_op : expr<Result>{
      typedef Result (*binary_fun)( Arg ,Arg);
      binary_op(binary_fun fun_in,expr<Arg> * left_in, expr<Arg> * right_in)
      :m_fun{fun_in}, m_left{(expr<Arg>*)left_in->fold()},m_right{(expr<Arg>*)right_in->fold()}{}
      ~binary_op(){delete m_left; delete m_right;}
      Result eval() const 
      {
         return m_fun(m_left->eval(), m_right->eval());
      }
      bool is_constant() const { return m_left->is_constant() && m_right->is_constant();}
      expr<Result>* fold()
      {
         if (this->is_constant()){
            auto * result = new constant<Result>(this->eval());
            delete this;
            return result;
         }else{
            return this;
         }
      }

      expr<Result>* clone() const  
      {
         return new binary_op{this->m_fun,(expr<Arg>*)this->m_left->clone(),(expr<Arg>*)this->m_right->clone()};
      }
   private:
      binary_fun  m_fun;
      expr<Arg> * m_left;
      expr<Arg> * m_right;
   };

   template <typename T>
   struct if_op : expr<T>{
         if_op ( expr<bool> * cond_in, expr<T>* true_expr_in, expr<T>* false_expr_in)
         : m_condition{(expr<bool> *)cond_in->fold()}, m_true_expr{(expr<T>*)true_expr_in->fold()},m_false_expr{(expr<T>*)false_expr_in->fold()}{}
         ~if_op(){delete m_condition; if(m_true_expr){delete m_true_expr;} if (m_false_expr){delete m_false_expr;} }
        T eval() const
        {
            if ( m_condition->eval()){
               return m_true_expr->eval();
            }else{
               return m_false_expr->eval();
            }
        }

        bool is_constant() const 
        { 
            if ( m_condition->is_constant()){
               if ( m_condition->eval() ){
                  return  m_true_expr->is_constant();
               }else{
                  return  m_false_expr->is_constant();
               }
            }else{
               return false;
            }
         }

         expr<T> * fold()
         {
             if ( m_condition->is_constant()){
               
               expr<T>* & active_expr = (m_condition->eval())? m_true_expr: m_false_expr;
               if ( active_expr->is_constant()){
                  auto * result = new constant<T>(active_expr->eval());
                  delete this;
                  return result;
               }else{
                  auto* result = active_expr;
                  active_expr = nullptr;
                  delete this;
                  return result;
               }
            }else{
               return this;
            }
         }
         expr<T>* clone() const  
         {
            return new if_op{(expr<bool> *)this->m_condition->clone(),(expr<T>*)this->m_true_expr->clone(),(expr<T>*)this->m_false_expr->clone()};
         }
      private:
         expr<bool> * m_condition;
         expr<T>* m_true_expr;
         expr<T>* m_false_expr;
   };

}

#endif // MIXER_LANG_EXPRTREE_HPP_INCLUDED
