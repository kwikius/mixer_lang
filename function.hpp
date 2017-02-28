#ifndef MIXER_LANG_FUNCTION_HPP_INCLUDED
#define MIXER_LANG_FUNCTION_HPP_INCLUDED

#include "abc_expr.hpp"

namespace apm_mix{

   struct abc_expr;

   struct arg_list{
      abc_expr * m_arg;
      arg_list * m_next;
   };

   struct function_builder {
      function_builder(arg_list * args) : m_arg_list{args}{ }
      virtual ~ function_builder()
      { 
         while ( m_arg_list != nullptr ){
            auto* temp = m_arg_list;
            m_arg_list = m_arg_list->m_next;
            delete temp;
         }
      }
      arg_list* m_arg_list;
      // get_result_type
      virtual bool type_check_args() const = 0;
      
      virtual abc_expr::exprID get_result_type() const = 0;
   };
}

#endif // MIXER_LANG_FUNCTION_HPP_INCLUDED
