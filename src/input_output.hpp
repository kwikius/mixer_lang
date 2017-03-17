#ifndef MIXER_LANG_INPUT_OUTPUT_HPP_INCLUDED
#define MIXER_LANG_INPUT_OUTPUT_HPP_INCLUDED

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

#include "expr.hpp"

namespace apm_mix{

   template <typename T>
   struct input final : expr<T>{
      typedef T ( *pfn_get) ();
      T eval()const {return m_pfn_get();}
      bool is_constant() const {return false;}
      expr<T>* fold() { return this;}
      expr<T>* clone() const { return new input{this->m_pfn_get};}
      input(pfn_get get_fn_in)
      : m_pfn_get{get_fn_in}{}
      ~input(){}
      private:
         pfn_get m_pfn_get;
   };

   template <typename T>
   struct output final : expr<T> {

      typedef void (*pfn_send)( T const &);
      output(pfn_send put_fn_in):m_pfn_send{put_fn_in}, m_expr{nullptr}{}
      // dont delete output expressions
      // The output is owned by the app, but its expressions are owned by the mixer
      ~output() { /*delete m_expr; */}
      T eval()const 
      { 
         if ( m_expr) { 
            return m_expr->eval();
         }else {
            return T{0} ;
         }
      }
      void zero_expr(){ m_expr = nullptr;}

      bool has_output_expr() const { return m_expr != nullptr;}
      bool set_output_expr(expr<T> * e) 
      {
         if (m_expr == nullptr){
            m_expr = e; 
            return true;
         }else{ 
            return false;
         }
      }

      bool is_constant() const { return false;}
      expr<T>* fold() { if ( m_expr) {m_expr = (expr<T>*)m_expr->fold();} return this;}
      expr<T>* clone() const { return new output{this->m_pfn_send, m_expr};}
      void apply() { m_pfn_send(this->eval());}
      private:
      // called by clone
      output(pfn_send put_fn_in, expr<T> * e)
      :m_pfn_send{put_fn_in}, m_expr{e}{}
      pfn_send m_pfn_send;
      expr<T> * m_expr;
   };

} // apm_mix

#endif // MIXER_LANG_INPUT_OUTPUT_HPP_INCLUDED
