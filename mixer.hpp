#ifndef MIXER_LANG_MIXER_HPP_INCLUDED
#define MIXER_LANG_MIXER_HPP_INCLUDED

#include "exprtree.hpp"
#include <cstring>

namespace apm_mix{
   
   struct abc_expr;
   struct symtab_t;

   struct input_pair{
      template <typename T>
      input_pair(const char* name, T (*pfn)() ): m_input{new input<T>{pfn}}, m_name{name}{}
      abc_expr* m_input;
      const char * m_name;
   };

   // make type of outputs a template param ?
   struct mixer_t{

      static constexpr uint32_t num_outputs = 18;
      
      mixer_t(input_pair* inputs, uint32_t num_inputs)
      :m_inputs{inputs}, m_num_inputs{num_inputs},m_expressions{nullptr}
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
      abc_expr* find_input(const char* name)
      {
         for (uint32_t i = 0; i < m_num_inputs; ++i){
            if ( strcmp(m_inputs[i].m_name, name ) == 0 ){
               return m_inputs[i].m_input;
            }
         }
         return nullptr;
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
      input_pair * m_inputs;
      uint32_t m_num_inputs;
      abc_expr* m_expressions;
      abc_expr* m_outputs[num_outputs] ;

   };

   void mixer_init(input_pair* inputs, uint32_t num_inputs);
   void eval_mixer_outputs();
}

#endif // MIXER_LANG_MIXER_HPP_INCLUDED
