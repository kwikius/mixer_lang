
#include <cstdio>
#include "mixer.hpp"

apm_mix::mixer_t* fn_mix();

int main()
{
   printf("Hello World\n");
   
   bool result = fn_mix();
   printf("Done %i\n", result);
   if (result){
     apm_mix::eval_mixer_outputs();
   }
}





