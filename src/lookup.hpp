#ifndef MIXER_LANG_SYMTAB_HPP_INCLUDED
#define MIXER_LANG_SYMTAB_HPP_INCLUDED

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
#include "stringfun.hpp"

namespace apm_mix{
   
   template <typename Node>
   struct lookup_t;

   template <typename Node>
   struct lookup_item{
     private:
       // symtab owns name
       //  but not node
       lookup_item(const char* name_in, Node node_in)
       : m_name{name_in}, m_node{node_in}, m_next{nullptr}{}
       ~lookup_item(){ delete_string(m_name); }
       const char* m_name;
       Node m_node;
       lookup_item<Node>* m_next;
       friend class lookup_t<Node> ;
   };

   // 
   template <typename Node>
   struct lookup_t{
      lookup_t(): m_first{nullptr}{}
      ~lookup_t()
      {
         while ( m_first != nullptr ){
            auto* temp = m_first;
            m_first = m_first->m_next;
            delete temp;
         }
      }
      Node find_item( const char* name); 
      // preconditions, name is not in the symtab.
      // do check name isnt in there first!
      void add_item(const char* name, Node node);
   private:
      lookup_item<Node> * m_first;
   };
  
} // apm_mix

#endif // MIXER_LANG_SYMTAB_HPP_INCLUDED
