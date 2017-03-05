#ifndef MIXER_LANG_EXPR_HPP_INCLUDED
#define MIXER_LANG_EXPR_HPP_INCLUDED

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

#include "abc_expr.hpp"
#include "mixer_lang_types.hpp"

namespace apm_mix{

   namespace detail{

      template <typename T>
      struct get_type_id;

      template<>
      struct get_type_id<apm_mix::int_t>{
         static constexpr auto value = abc_expr::exprID::INT;
      };

      template<>
      struct get_type_id<apm_mix::float_t>{
         static constexpr auto value = abc_expr::exprID::FLOAT;
      };

      template<>
      struct get_type_id<bool>{
         static constexpr auto value = abc_expr::exprID::BOOL;
      };
   }
      
   template <typename T>
   struct expr: abc_expr{
      virtual T eval() const =0;
      expr():abc_expr{apm_mix::detail::get_type_id<T>::value}{}
      expr & operator = (expr const &) = delete;
      expr (expr const &) = delete;
   };

}

#endif // MIXER_LANG_EXPR_HPP_INCLUDED
