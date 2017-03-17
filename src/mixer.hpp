#ifndef MIXER_LANG_MIXER_HPP_INCLUDED
#define MIXER_LANG_MIXER_HPP_INCLUDED

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
#include <cstring>
#include "input_pair.hpp"
#include "exprtree.hpp"
#include "arg_list.hpp"
#include "error.hpp"

namespace apm_mix{

   template <typename Node>
   struct lookup_t;

   // TODO destructor for mixer
   struct mixer_t{
      mixer_t(input_pair* inputs, uint32_t num_inputs, output<float_t> * outputs , uint32_t num_outputs )
         :m_inputs{inputs}
         ,m_num_inputs{num_inputs}
         ,m_outputs{outputs}
         ,m_num_outputs{num_outputs}
         ,m_expressions{nullptr}
      {}
       ~mixer_t()
       {
          delete_args(m_expressions);
          m_expressions = nullptr;
       }
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

         if ( output_expr->get_ID() != abc_expr::exprID::FLOAT){
            yyerror("output expression must be float");
            return false;
         }

         auto & out = m_outputs[n];
         out.set_output_expr((expr<apm_mix::float_t>*) output_expr);
         return true;

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
            if ( m_outputs[i].has_output_expr()){
               m_outputs[i].apply();
            }
         }
      }
    private:
      input_pair * m_inputs;
      uint32_t const m_num_inputs;
      output<float_t> * m_outputs;
      uint32_t const m_num_outputs;
      arg_list* m_expressions;
   };

   bool mixer_create(apm_lexer::stream_t * ,
     input_pair* inputs, uint32_t num_inputs, output<float_t> *outputs , uint32_t num_outputs);
   void close_mixer();
   void mixer_eval();
}

#endif // MIXER_LANG_MIXER_HPP_INCLUDED
