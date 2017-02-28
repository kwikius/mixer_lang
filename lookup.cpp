
#include "lookup.hpp"
#include <cstring>

template <typename Node>
apm_mix::lookup_item<Node> * apm_mix::lookup_t<Node>::find_item( const char* name)
{
  lookup_item<Node>* ptr = m_first;
  while(ptr){
      if ( strcmp(name,ptr->m_name) ==0){
         return ptr;
      }else{
         ptr = ptr->m_next;
      }
  }
  return nullptr;
}

template <typename Node>
void apm_mix::lookup_t<Node>::add_item(const char* name, Node* node)
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

