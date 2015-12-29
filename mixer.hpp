#ifndef MIXER_LANG_MIXER_HPP_INCLUDED
#define MIXER_LANG_MIXER_HPP_INCLUDED

#include "exprtree.hpp"

namespace apm_mix{
   
   struct abc_expr;
   struct symtab_t;

   // make type of outputs a template param ?
   struct mixer_t{

      static constexpr uint32_t num_outputs = 18;
      mixer_t():m_expressions{nullptr}
      {
         for ( uint32_t i = 0; i < num_outputs; ++i){
            m_outputs[i] = nullptr;
         }
      }
      void add(abc_expr* expr)
      {
         if ( m_expressions == nullptr){
            m_expressions = expr;
         }else{
            expr->m_next = m_expressions;
            m_expressions = expr;
         }
      }
      bool add_output(uint32_t n, abc_expr* output_expr)
      {
          
          if ( n > num_outputs)  {
             yyerror("invalid output index");
             return false;
          }
          if ( m_outputs[n] != nullptr){
            yyerror("output already assigned");
            return false;
          }
          m_outputs[n] = output_expr;
          return true;
      }
      void eval_outputs()
      {
         for ( uint32_t i = 0; i < num_outputs; ++i){
            if ( m_outputs[i] != nullptr){
               switch ( m_outputs[i]->get_ID()){
                  case abc_expr::exprID::FLOAT:{
                     auto * out = (expr<double> *)m_outputs[i];
                     printf("output[%u] = %f\n",static_cast<unsigned>(i),out->eval());
                     break;
                  }
                  case abc_expr::exprID::INT:{
                     auto * out = (expr<int64_t> *)m_outputs[i];
                     printf("output[%u] = %d\n",static_cast<unsigned>(i),static_cast<int>(out->eval()));
                     break;
                  }
                  case abc_expr::exprID::BOOL:{
                     auto * out = (expr<bool> *)m_outputs[i];
                     char const * result = (out->eval())? "true": "false";
                     printf("output[%u] = %s\n",static_cast<unsigned>(i),result);
                     break;
                  }
                  default:
                     printf("unknown value\n");
                     break;
               }
            }
         }
      }
    private:
      abc_expr* m_expressions;
      abc_expr* m_outputs[num_outputs] ;
   };

   void mixer_init();
   void eval_mixer_outputs();
}

#endif // MIXER_LANG_MIXER_HPP_INCLUDED
