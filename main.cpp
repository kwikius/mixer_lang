
#include <quan/operators/binary_operator_functors.hpp>

namespace apm_mix{

   template <typename T>
   struct expr{
      virtual T eval() const =0;
      virtual ~ expr(){}
   };

   template <typename T>
   struct literal : expr<T>{
      literal(T value_in):m_value{value_in}{}
      T eval()const {return m_value;}
    private:
      T m_value;
   };

   template <typename T>
   struct variable : expr<T>{
      variable(expr<T>* expr_in): m_expr(expr_in){}
      T eval() const { return m_expr->eval();}
      variable & operator = (expr<T>* expr_in)
      {
         m_expr = expr_in;
         return *this;
      }
      private:
      expr<T>* m_expr;
   };

   template <typename Result , typename Arg>
   struct unary_op : expr<Result>{
      typedef Result (*unary_fun)( Arg);
      unary_op(unary_fun fun_in,expr<Arg> * arg_in)
      :m_fun{fun_in}, m_arg{arg_in}{}
      Result eval() const 
      {
         return m_fun(m_arg->eval());
      }
   private:
      unary_fun  m_fun;
      expr<Arg> * m_arg;
   };

   template <typename Result , typename Arg>
   struct binary_op : expr<Result>{
      typedef Result (*binary_fun)( Arg ,Arg);
      binary_op(binary_fun fun_in,expr<Arg> * left_in, expr<Arg> * right_in)
      :m_fun{fun_in}, m_left{left_in},m_right{right_in}{}
      Result eval() const 
      {
         return m_fun(m_left->eval(), m_right->eval());
      }
   private:
      binary_fun  m_fun;
      expr<Arg> * m_left;
      expr<Arg> * m_right;
   };

   template <typename T>
   struct if_op : expr<T>{
         if_op ( expr<bool> * cond_in, expr<T>* true_expr_in, expr<T>* false_expr_in)
         : m_condition{cond_in}, m_true_expr{true_expr_in},m_false_expr{false_expr_in}{}

        T eval() const
        {
            if ( m_condition->eval()){
               return m_true_expr->eval();
            }else{
               return m_false_expr->eval();
            }
        }
      private:
         expr<bool> * m_condition;
         expr<T> * m_true_expr;
         expr<T>* m_false_expr;
   };

   template <typename Op,typename Lhs, typename Rhs>
   typename quan::operator_<Op>:: template apply<Lhs,Rhs>::type
   binary_fun(Lhs  lhs, Rhs  rhs)
   {
      return quan::operator_<Op>{}(lhs,rhs);
   }
}

#include <iostream>

using namespace apm_mix;

int main()
{
   literal<float> one{1};
   literal<float> two{2};
   literal<float> three{3};

   literal<bool> false_{false};
   literal<bool> true_{true};

   variable<float> var{&one};
    
   binary_op<float,float> plus{&binary_fun<quan::meta::plus,float,float>, &var,&one};
   binary_op<bool,float> less{&binary_fun<quan::meta::less,float,float>, &var,&three};
   
   std::cout  << plus.eval() <<'\n';
   std::cout << std::boolalpha << less.eval() <<'\n';
   var = &three;
   std::cout  << plus.eval() <<'\n';
   std::cout << std::boolalpha << less.eval() <<'\n';

   if_op<float> if1{&true_,&plus,&two};

   std::cout << if1.eval();

   
}





