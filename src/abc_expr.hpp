#ifndef MIXER_LANG_ABC_EXPR_HPP_INCLUDED
#define MIXER_LANG_ABC_EXPR_HPP_INCLUDED

#include <cstdint>

namespace apm_mix{

   struct mixer_t;

   struct abc_expr{
      enum class exprID : uint8_t {BOOL,INT,FLOAT} ;
      
      exprID get_ID()const { return m_id;}  
      virtual bool is_constant() const = 0;
      virtual abc_expr* fold () = 0;
      virtual abc_expr* clone() const =0; 
      virtual ~abc_expr(){}
    protected:
       abc_expr(exprID id_in): m_id{id_in} {}
     private:
      exprID const m_id;
   };

}

#endif // MIXER_LANG_ABC_EXPR_HPP_INCLUDED
