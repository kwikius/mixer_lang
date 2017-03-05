#ifndef MIXER_LANG_HPP_INCLUDED
#define MIXER_LANG_HPP_INCLUDED

#include "../src/input_pair.hpp"

namespace apm_mix{

   bool mixer_create(const char * filename,
   input_pair* inputs, uint32_t num_inputs, abc_expr ** outputs , uint32_t num_outputs);

   void mixer_eval();
}

#endif // MIXER_LANG_HPP_INCLUDED
