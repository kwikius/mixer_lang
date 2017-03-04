#ifndef MIXER_LANG_MIXER_HPP_INCLUDED
#define MIXER_LANG_MIXER_HPP_INCLUDED

#include "exprtree.hpp"
#include "arg_list.hpp"
#include <cstring>

namespace apm_mix{
   
   struct abc_expr;

   template <typename Node>
   struct lookup_t;

   struct input_pair{
      template <typename T>
      input_pair(const char* name, T (*pfn)() ): m_input{new input<T>{pfn}}, m_name{name}{}
      abc_expr* m_input;
      const char * m_name;
   };

   // TODO destructor for mixer
   struct mixer_t{
      mixer_t(input_pair* inputs, uint32_t num_inputs, abc_expr ** outputs , uint32_t num_outputs )
         :m_inputs{inputs}
         ,m_num_inputs{num_inputs}
         ,m_outputs{outputs}
         ,m_num_outputs{num_outputs}
         ,m_expressions{nullptr}
      {}

      /*
         add an l_value ( assignment expression , named expression)
         to the list of expressions in the mixer.
       */
      void add(abc_expr* expr)
      {
         add_arg(m_expressions,expr);
      }

      /*
         add an output expression to the mixer
         if is an unfilled slot for it,
         else return false;
      */
      bool add_output(uint32_t n, abc_expr* output_expr)
      {
         if ( n >= m_num_outputs)  {
            yyerror("invalid output index");
            return false;
         }

         if ( output_expr->get_ID() != m_outputs[n]->get_ID()){
            yyerror("no match for output expr type for assign");
            return false;
         }

         switch ( m_outputs[n]->get_ID()){
            case abc_expr::exprID::BOOL:{
               auto * out = (output<bool>*)m_outputs[n];
               out->set_output_expr( (expr<bool>*)output_expr);
               return true;
            }
            case abc_expr::exprID::INT:{
               auto * out = (output<int64_t>*)m_outputs[n];
               out->set_output_expr( (expr<int64_t>*) output_expr);
               return true;
            }
            case abc_expr::exprID::FLOAT:{
               auto * out = (output<double>*)m_outputs[n];
               out->set_output_expr((expr<double>*) output_expr);
               return true;
            }
            default:
               yyerror("unexpected");
               return false;
         }
      }

      /*
         search the list of input_expressions (provided to the mixer when built)
         for the one identified by name and return a ptr if found, else nullptr
      */
      abc_expr* find_input(const char* name)const
      {
         for (uint32_t i = 0; i < m_num_inputs; ++i){
            if ( strcmp(m_inputs[i].m_name, name ) == 0 ){
               return m_inputs[i].m_input;
            }
         }
         return nullptr;
      }

      /*
        The whole point of the mixer!
        Evaluate the outputs periodically and send the results 
        of the attached expresions to the output functions
        It might be faster to make this homogeneous types only
        but its lookup is probably not much overhead
      */
      void eval_outputs()
      {
         for ( uint32_t i = 0; i < m_num_outputs; ++i){
            if ( m_outputs[i] != nullptr){
               switch ( m_outputs[i]->get_ID()){
                  case abc_expr::exprID::FLOAT:{
                     auto * out = (output<double> *)m_outputs[i];
                     out->apply();
                     break;
                  }
                  case abc_expr::exprID::INT:{
                     auto * out = (output<int64_t> *)m_outputs[i];
                     out->apply();
                     break;
                  }
                  case abc_expr::exprID::BOOL:{
                     auto * out = (output<bool> *)m_outputs[i];
                     out->apply();
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
      uint32_t const m_num_inputs;
      abc_expr ** m_outputs;
      uint32_t const m_num_outputs;
      arg_list* m_expressions;
   };

   bool mixer_create(const char * filename,
     input_pair* inputs, uint32_t num_inputs, abc_expr ** outputs , uint32_t num_outputs);
   void mixer_eval();
}

#endif // MIXER_LANG_MIXER_HPP_INCLUDED
