#ifndef MIXER_LANG_SYMTAB_HPP_INCLUDED
#define MIXER_LANG_SYMTAB_HPP_INCLUDED

namespace apm_mix{

   template <typename Node>
   struct lookup_item{
       // symtab owns name but not node
       lookup_item(const char* name_in, Node node_in)
       : m_name{name_in}, m_node{node_in}, m_next{nullptr}{}
       // currently wrong so 
       // TODO memory management for name
       // currently created with strdup
       ~lookup_item(){ delete  m_name; }
       const char* m_name;
       Node m_node;
       lookup_item<Node>* m_next;
   };

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
