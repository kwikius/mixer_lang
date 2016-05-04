
#include <cstdio>
#include "mixer.hpp"
#include "lexer.hpp"
#include <quan/utility/timer.hpp>

apm_mix::mixer_t* fn_mix();

namespace {
  double pitch = 0.0;
  double roll = 0.1;
  double yaw = 0.0; 
  double throttle = 0.345;
  double airspeed = 20.0;
  double flap = -1;
}
double get_pitch(){ return pitch;}
double get_yaw(){ return yaw;}
double get_roll(){ return roll;}
double get_throttle(){ return throttle;}
double get_airspeed(){ return airspeed;}
double get_flap(){ return flap;}

// TODO make inputs optionally constant
apm_mix::input_pair inputs[]
 = { 
      apm_mix::input_pair{"Pitch",  static_cast<double(*)()>(get_pitch)},
      apm_mix::input_pair{"Yaw",  static_cast<double(*)()>(get_yaw)},
      apm_mix::input_pair{"Roll", static_cast<double(*)()>(get_roll)},
      apm_mix::input_pair{"Throttle", static_cast<double(*)()>(get_throttle)},
      apm_mix::input_pair{"Flap", static_cast<double(*)()>(get_flap)},
      apm_mix::input_pair{"Airspeed", static_cast<double(*)()>(get_airspeed)},
      apm_mix::input_pair{"ARSPD_FBWA_MIN", static_cast<double(*)()>([]()->double{return 10.0;})}
   };

namespace {

   typedef quan::time::ms ms;

   ms operator "" _ms(unsigned long long v)
   {
      return static_cast<ms>(static_cast<double>(v));
   }
}

int main()
{
   // Assign the inputs.
   // These can be any functions returning a double
   // that provide the aircraft runtime values
   // such as pitch, roll airspeed etc.
   apm_mix::mixer_init(inputs, sizeof(inputs)/sizeof(inputs[0]));

   bool success = false;
   if ( apm_lexer::open_file("basic.mix")){
      // fn_mix builds the mixer from the input stream
      if (fn_mix()){
        // if the build is succesfull
       success = true;
      }
      apm_lexer::close_file();
   }else{
      printf("open file failed\n");
   }
   
   if (success){

      // start using the mixer
      quan::timer<> timer;
      bool roll_dir_positive = true;
      for (;;){
         if ( timer() >= 500_ms){
            timer.restart();
            printf("----------------\n");
            apm_mix::eval_mixer_outputs();
            if ( roll_dir_positive){
               if ( roll < 0.5){
                  roll += 0.1;
               }else{
                 roll_dir_positive = false; 
               }
            }else{
              if ( roll > -0.5){
                  roll -= 0.1;
               }else{
                 roll_dir_positive = true; 
               }
            }
         }
      }
   }
}





