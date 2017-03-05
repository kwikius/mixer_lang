#ifndef MIXER_LANG_ARG_LIST_HPP_INCLUDED
#define MIXER_LANG_ARG_LIST_HPP_INCLUDED

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

   struct abc_expr;

   struct arg_list{
      arg_list(abc_expr * arg_in, arg_list* next = nullptr)
      : m_arg{arg_in},m_next{next}{}
      abc_expr * m_arg;
      arg_list * m_next;
   };

   // 0 if empty
   int32_t get_num_elements( arg_list const* p);

   // no args or bigger than args return null
   // else zero based first arg is 0
   abc_expr* get_arg(arg_list const* p, uint32_t i);

   // push_back
   // n.b returns a new arg_list for nullptr p
   // always assign ret
   arg_list* add_arg(arg_list* p, abc_expr* expr);
   bool delete_args(arg_list* p);
}

#endif // MIXER_LANG_ARG_LIST_HPP_INCLUDED
