#ifndef MICER_LANG_EXPRTREE_HPP_INCLUDED
#define MICER_LANG_EXPRTREE_HPP_INCLUDED

#include <cstdint>
namespace apm_mix{

   bool yyerror(const char* str = nullptr);

   struct mixer_t;

   struct abc_expr{
      enum exprID{BOOL,INT,FLOAT} ;
      abc_expr(uint32_t id_in): m_id{id_in}, m_next{nullptr}{}
      uint32_t get_ID()const { return m_id;}  
      virtual bool is_constant() const = 0;
      virtual abc_expr* fold () = 0;
      virtual abc_expr* clone() const =0; 
      virtual ~abc_expr(){}
     private:
      uint32_t const m_id;
      abc_expr* m_next;
      friend class apm_mix:: mixer_t;
   };

   template <typename T>
   struct get_type_id;

   template<>
   struct get_type_id<int64_t>{
      static constexpr uint32_t value = abc_expr::exprID::INT;
   };

   template<>
   struct get_type_id<double>{
      static constexpr uint32_t value = abc_expr::exprID::FLOAT;
   };

   template<>
   struct get_type_id<bool>{
      static constexpr uint32_t value = abc_expr::exprID::BOOL;
   };
      
   template <typename T>
   struct expr: abc_expr{
      virtual T eval() const =0;
     // virtual ~ expr(){}

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
#if 0
   // is set up with a default expression
   template <typename T>
   struct mixer_output : expr<T>{
      typedef void ( *pfn) ( T );
      mixer_output(pfn fun, expr<T>* expr_in): m_output_fun{fun}, m_expr{expr_in->fold()}
      {}
      ~mixer_output(){ if (m_expr != nullptr){delete m_expr;}}
      T eval() const 
      { 
         return m_expr->eval();
      }

      bool is_constant() const 
      { 
         return m_expr->is_constant();
      }

      expr<T>* fold() 
      { 
         return this;
      }
      
      void set_expression( expr<T>* expr)
      {
         delete m_expr;
         m_expr = expr;
      }

      void do_output_fun()
      {
         m_output_fun(this->eval());
      }

      expr<T>* clone() const 
      { 
         return new mixer_output{this->m_output_fun,this->m_expr->clone()};
      }
      private:
      pfn m_output_fun;
      expr<T> * m_expr;
//      ~variable()
//      { if (m_expr != nullptr) delete m_expr;}
//      
//      expr<T>* fold() 
//      { 
//         if (this->is_constant()){
//       //     std:: cout << "folding var\n";
//            expr<T>* t = m_expr;
//            m_expr = nullptr;
//            delete this;
//            return t;
//         }else{
//            return this;
//         }
//      }
//      expr<T>* clone() const { return new variable{this->m_expr->clone()};}
   };
#endif
  
//   template <typename T>
//   struct variable : expr<T>{
//      variable(expr<T>* expr_in): m_expr(expr_in->fold()){}
//      ~variable()
//      { if (m_expr != nullptr) delete m_expr;}
//      T eval() const { return m_expr->eval();}
//      bool is_constant() const { return m_expr->is_constant();}
//      expr<T>* fold() 
//      { 
//         if (this->is_constant()){
//       //     std:: cout << "folding var\n";
//            expr<T>* t = m_expr;
//            m_expr = nullptr;
//            delete this;
//            return t;
//         }else{
//            return this;
//         }
//      }
//      expr<T>* clone() const { return new variable{this->m_expr->clone()};}
//   private:
//      expr<T>* m_expr;
//   };

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
        //     std:: cout << "folding unary\n";
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
           // std:: cout << "folding binary\n";
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
         : m_condition{cond_in->fold()}, m_true_expr{true_expr_in->fold()},m_false_expr{false_expr_in->fold()}{}
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
              //    std::cout << "if active folding \n"; 
                  auto * result = new constant<T>(active_expr->eval());
                  delete this;
                  return result;
               }else{
               //   std::cout << "if folding \n";
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
            return new if_op{this->m_condition->clone(),this->m_true_expr->clone(),this->m_false_expr->clone()};
         }
      private:
         expr<bool> * m_condition;
         expr<T> * m_true_expr;
         expr<T>* m_false_expr;
   };
}

#endif // MICER_LANG_EXPRTREE_HPP_INCLUDED
