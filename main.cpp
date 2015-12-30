
#include <cstdio>
#include "mixer.hpp"
#include "lexer.hpp"

apm_mix::mixer_t* fn_mix();

double get_pitch(){ return 0.5;}

// TODO make inputs optionally constant
apm_mix::input_pair inputs[]
 = { 
      apm_mix::input_pair{"Pitch",  static_cast<double(*)()>(get_pitch)},
      // example of lambda , seem to reuire static cast with c++11
      apm_mix::input_pair{"Roll", static_cast<double(*)()>([]()->double{return -0.5;})},
      apm_mix::input_pair{"Throttle", static_cast<double(*)()>([]()->double{return 10.0;})}
   };
int main()
{
   // add inputs
   apm_mix::mixer_init(inputs, sizeof(inputs)/sizeof(inputs[0]));
   if ( apm_lexer::open_file("example1.mix")){
      if (fn_mix()){
        apm_mix::eval_mixer_outputs();
      }
      apm_lexer::close_file();
   }else{
      printf("open file failed\n");
   }
}





