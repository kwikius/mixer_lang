#ifndef MIXER_LANG_HPP_INCLUDED
#define MIXER_LANG_HPP_INCLUDED

#include "../src/input_pair.hpp"
#include "../src/lexer_stream.hpp"

namespace apm_mix{

   bool mixer_create(
      apm_lexer::stream_t * pstream, // caller not mixer owns pstream
      input_pair* inputs, uint32_t num_inputs,  // caller owns inputs
      abc_expr ** outputs , uint32_t num_outputs // caller own outputs
   );

   void mixer_eval();
   void close_mixer(); // clean up
}

#endif // MIXER_LANG_HPP_INCLUDED
