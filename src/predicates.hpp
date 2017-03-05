#ifndef MIXER_LANG_PREDICATES_HPP_INCLUDED
#define MIXER_LANG_PREDICATES_HPP_INCLUDED

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

namespace apm_mix{

   struct abc_expr;

   bool is_bool_expr(abc_expr* ptr);

   bool are_boolean(abc_expr* lhs, abc_expr* rhs);

   bool is_float_expr(abc_expr* ptr);

   bool is_int_expr(abc_expr* ptr);

   bool is_numeric(abc_expr* ptr);

   bool are_numeric(abc_expr* lhs, abc_expr* rhs);

   bool are_same_type(abc_expr* lhs, abc_expr* rhs);

   bool are_same_numeric(abc_expr* lhs, abc_expr* rhs);

}
#endif // MIXER_LANG_PREDICATES_HPP_INCLUDED
