#ifndef MIXER_LANG_MIXER_HPP_INCLUDED
#define MIXER_LANG_MIXER_HPP_INCLUDED


#include "exprtree.hpp"

namespace apm_mix{
   
   struct output_expr;
   struct abc_expr;
   struct symtab_t;

   struct mixer_t{
      mixer_t():m_expressions{nullptr}{}
      void add(abc_expr* expr)
      {
         if ( m_expressions == nullptr){
            m_expressions = expr;
         }else{
            expr->m_next = m_expressions;
            m_expressions = expr;
         }
      }
      void add_output(output_expr*)
      {
            
      }
    private:
      abc_expr* m_expressions;
      output_expr* m_outputs;
   };


}

#endif // MIXER_LANG_MIXER_HPP_INCLUDED
