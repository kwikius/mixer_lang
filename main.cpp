
#include <cstdio>
#include "mixer.hpp"

apm_mix::mixer_t* fn_mix();

int main()
{
   printf("mixer demo\nInput the mixer...\n\n");
   
   if (fn_mix()){
     apm_mix::eval_mixer_outputs();
   }
}





