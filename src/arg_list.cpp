
#include "abc_expr.hpp"
#include "arg_list.hpp"

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

apm_mix::arg_list* apm_mix::add_arg(arg_list* args, abc_expr* expr)
{
   if ( args == nullptr){
      return new arg_list{expr};
   }else{
      auto * temp = args;
      while ( temp->m_next != nullptr){
         temp = temp->m_next;
      }
      temp->m_next = new arg_list{expr};
      return args;
   }
}

int32_t apm_mix::get_num_elements(arg_list const* args)
{
   int arg_count = 0;
   while ( args != nullptr){
      args = args->m_next;
      ++arg_count;
   }
   return arg_count;     
}

bool apm_mix::delete_args(apm_mix::arg_list* args)
{
   while (args != nullptr){
      auto * temp = args;
      args = args->m_next;
      delete temp;
   }
   return true;
}
  
apm_mix::abc_expr * apm_mix::get_arg(arg_list const* args, uint32_t i)
{
   uint32_t arg_count = 0;
   while ( args != nullptr){
      if (arg_count == i){
         return args->m_arg;
      }else{
         args = args->m_next;
         ++arg_count;
      }
   }
   return nullptr;
}
