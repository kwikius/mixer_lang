#ifndef MIXER_LANG_ABC_EXPR_HPP_INCLUDED
#define MIXER_LANG_ABC_EXPR_HPP_INCLUDED

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

#include <cstdint>

namespace apm_mix{

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
