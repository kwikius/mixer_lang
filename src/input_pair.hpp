#ifndef MIXER_LANG_INPUT_PAIR_HPP_INCLUDED
#define MIXER_LANG_INPUT_PAIR_HPP_INCLUDED

#include "input_output.hpp"

namespace apm_mix{

   struct input_pair{
      template <typename T>
      input_pair(const char* name, T (*pfn)() ): m_input{new input<T>{pfn}}, m_name{name}{}
      abc_expr* m_input;
      const char * m_name;
   };

}

#endif // MIXER_LANG_INPUT_PAIR_HPP_INCLUDED
