
#include <cstdio>
#include "mixer.hpp"
#include "lexer.hpp"
#include <quan/utility/timer.hpp>

apm_mix::mixer_t* fn_mix();

namespace {
  double pitch = 0.0;
  double roll = 0.5;
  double yaw = 0.0; 
  double throttle = 0.0; 
  double airspeed = 20.0;
  double flap = 0.3;
  double control_mode = 0.3; // flap
}
double get_pitch(){ return pitch;}
double get_yaw(){ return yaw;}
double get_roll(){ return roll;}
double get_throttle(){ return throttle;}
double get_airspeed(){ return airspeed;}
double get_flap(){ return flap;}
double get_control_mode(){return control_mode;}

// TODO make inputs optionally constant
apm_mix::input_pair inputs[]
 = { 
      apm_mix::input_pair{"Pitch",  static_cast<double(*)()>(get_pitch)},
      apm_mix::input_pair{"Yaw",  static_cast<double(*)()>(get_yaw)},
      apm_mix::input_pair{"Roll", static_cast<double(*)()>(get_roll)},
      apm_mix::input_pair{"Throttle", static_cast<double(*)()>(get_throttle)},
      apm_mix::input_pair{"Flap", static_cast<double(*)()>(get_flap)},
      apm_mix::input_pair{"Airspeed", static_cast<double(*)()>(get_airspeed)},
      apm_mix::input_pair{"ControlMode", static_cast<double(*)()>(get_control_mode)},
      apm_mix::input_pair{"ARSPD_FBWA_MIN", static_cast<double(*)()>([]()->double{return 10.0;})},
      apm_mix::input_pair{"ID", static_cast<int64_t(*)()>([]()->int64_t{return 999;})},
      apm_mix::input_pair{"boolean", static_cast<bool(*)()>([]()->bool{return true;})}
   };

int main(int argc , char* argv[])
{
   if ( argc < 2){
      printf("Useage : %s <mixer_filename>\n",argv[0]);
      return EXIT_SUCCESS;
   }
   // Assign the inputs.
   // These can be any functions returning a double
   // that provide the aircraft runtime values
   // such as pitch, roll airspeed etc.
   apm_mix::mixer_init(inputs, sizeof(inputs)/sizeof(inputs[0]));

   bool success = false;
   if ( apm_lexer::open_file(argv[1])){
      // fn_mix builds the mixer from the input stream
      if (fn_mix()){
        // if the build is succesfull
       success = true;
      }
      apm_lexer::close_file();
   }else{
      printf("open file %s failed\n",argv[1]);
   }
   
   if (success){
      bool roll_dir_positive = true;
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





