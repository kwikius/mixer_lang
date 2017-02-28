#include "exprtree.hpp"
#include "symtab.hpp"
#include <cstring>

using namespace apm_mix;

symtab_item * symtab_t::find_item( const char* name)
{
  symtab_item* ptr = m_first;
  while(ptr){
      if ( strcmp(name,ptr->m_name) ==0){
         return ptr;
      }else{
         ptr = ptr->m_next;
      }
  }
  return nullptr;
}

void symtab_t::add_item(const char* name, abc_expr* node)
{
   if ( m_first != nullptr){
       auto * temp = m_first;
       while ( temp->m_next != nullptr){
         temp = temp->m_next;
       }
       temp->m_next = new symtab_item{name, node};
   }else{
      m_first = new symtab_item{name, node};
   }
}
