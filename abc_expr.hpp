#ifndef MIXER_LANG_ABC_EXPR_HPP_INCLUDED
#define MIXER_LANG_ABC_EXPR_HPP_INCLUDED

#include <cstdint>

namespace apm_mix{

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

}

#endif // MIXER_LANG_ABC_EXPR_HPP_INCLUDED
