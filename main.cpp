
#include <cstdio>
#include "mixer.hpp"
#include "lexer.hpp"

apm_mix::mixer_t* fn_mix();

int main()
{
   apm_mix::mixer_init();
   if ( apm_lexer::open_file("example1.mix")){
      if (fn_mix()){
        apm_mix::eval_mixer_outputs();
      }
      apm_lexer::close_file();
   }else{
      printf("open file failed\n");
   }
}





