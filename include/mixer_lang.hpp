#ifndef MIXER_LANG_HPP_INCLUDED
#define MIXER_LANG_HPP_INCLUDED

#include "../src/input_pair.hpp"
#include "../src/lexer_stream.hpp"
#include "../src/error.hpp"
#include "../src/stringfun.hpp"

namespace apm_mix{

   bool mixer_create(
      apm_lexer::stream_t * pstream, // caller not mixer owns pstream
      input_pair* inputs, uint32_t num_inputs,  // caller owns inputs
      output<float_t> * outputs , uint32_t num_outputs // caller own outputs
   );

   void mixer_eval();
   void close_mixer(); // clean up

   //N.B. the app is required to implement this
   // always returns false
   bool yyerror(const char* str );
}

// for implementing yyerror. The line number of the error
namespace apm_lexer{
  int get_line_number();

}

#endif // MIXER_LANG_HPP_INCLUDED
