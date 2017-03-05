
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

#include "lookup.hpp"
#include <cstring>

template <typename Node>
Node apm_mix::lookup_t<Node>::find_item( const char* name)
{
  lookup_item<Node>* ptr = m_first;
  while(ptr){
      if ( strcmp(name,ptr->m_name) ==0){
         return ptr->m_node;
      }else{
         ptr = ptr->m_next;
      }
  }
  return nullptr;
}

template <typename Node>
void apm_mix::lookup_t<Node>::add_item(const char* name, Node node)
{
   if ( m_first != nullptr){
       auto * temp = m_first;
       while ( temp->m_next != nullptr){
         temp = temp->m_next;
       }
       temp->m_next = new lookup_item<Node>{name, node};
   }else{
      m_first = new lookup_item<Node>{name, node};
   }
}

