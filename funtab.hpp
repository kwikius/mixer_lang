#ifndef MIXER_LANG_FUNTAB_HPP_INCLUDED
#define MIXER_LANG_FUNTAB_HPP_INCLUDED

#include "function.hpp"

namespace apm_mix{

 struct funtab_item_t{

    funtab_item_t(const char* name_in, function_builder * builder_in)
       : m_name{name_in}, m_builder{builder_in}, m_next{nullptr}{}
       ~funtab_item_t(){ delete m_name; }
       const char* m_name;
       function_builder* m_builder;
       funtab_item_t* m_next;
   };

   struct funtab_t{
      funtab_t(): m_first{nullptr}{}
      ~funtab_t()
      {
         while ( m_first != nullptr ){
            auto* temp = m_first;
            m_first = m_first->m_next;
            delete temp;
         }
      }
      funtab_item_t* find_item( const char* name); 

      // preconditions, name is not in the funtab.
      // do check name isnt in there first!
      void add_item(const char* name, funtab_item_t * node);
   private:
      funtab_item_t * m_first;
   };
}

#endif // MIXER_LANG_FUNTAB_HPP_INCLUDED
