#ifndef MIXERLANG_SYMTAB_HPP_INCLUDED
#define MIXERLANG_SYMTAB_HPP_INCLUDED

#include "exprtree.hpp"

namespace apm_mix{

   struct symtab_item{
       // symtab owns name but not node
       symtab_item(const char* name_in, abc_expr* node_in)
       : m_name{name_in}, m_node{node_in}, m_next{nullptr}{}
       ~symtab_item(){ delete m_name; }
       const char* m_name;
       abc_expr * m_node;
       symtab_item* m_next;
   };

   struct symtab_t{
      symtab_t(): m_first{nullptr}{}
      ~symtab_t()
      {
         while ( m_first != nullptr ){
            auto* temp = m_first;
            m_first = m_first->m_next;
            delete temp;
         }
      }

      symtab_item * find_item( const char* name); 

      // preconditions, name is not in the symtab.
      // do check name isnt in there first!
      void add_item(const char* name, abc_expr* m_node);
   private:
      symtab_item * m_first;
      //symtab_item * m_last;
   };
  
} // apm_mix

#endif // MIXERLANG_SYMTAB_HPP_INCLUDED
