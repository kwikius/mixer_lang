
#include <cstdio>
#include "../../src/mixer.hpp"
#include "../../src/lexer.hpp"
#include "joystick.hpp"
#include <cstdio>

namespace {

   double airspeed = 0.0; // test pathological on airspeed based mixer
   bool in_failsafe = false;  //
   double get_airspeed(){ return airspeed;}

   // TODO make inputs optionally constant
   apm_mix::input_pair inputs[]
    = { 
         apm_mix::input_pair{"Pitch", static_cast<double(*)()>(get_pitch)},
         apm_mix::input_pair{"Yaw",  static_cast<double(*)()>(get_yaw)},
         apm_mix::input_pair{"Roll", static_cast<double(*)()>(get_roll)},
         apm_mix::input_pair{"Throttle", static_cast<double(*)()>(get_throttle)},
         apm_mix::input_pair{"Flap", static_cast<double(*)()>(get_flap)},
         apm_mix::input_pair{"Airspeed", static_cast<double(*)()>(get_airspeed)},
         apm_mix::input_pair{"ControlMode", static_cast<double(*)()>(get_control_mode)},
         apm_mix::input_pair{"ARSPD_MIN", static_cast<double(*)()>([]()->double{return 10.0;})},
         apm_mix::input_pair{"ARSPD_CRUISE",static_cast<double(*)()>([]()->double{return 12.0;})},
         apm_mix::input_pair{"ARSPD_MAX", static_cast<double(*)()>([]()->double{return 20.0;})},
         apm_mix::input_pair{"FAILSAFE_ON", static_cast<bool(*)()>([]()->bool{return in_failsafe;})}
      };

   template<unsigned N>
   void action(double const & v)
   {
      printf("!!! output[%u] = %f\n",N,v);
   }

   apm_mix::abc_expr* outputs[]
   = {
      new apm_mix::output<double>{action<0>}
     , new apm_mix::output<double>{action<1>}
     , new apm_mix::output<double>{action<2>}
     , new apm_mix::output<double>{action<3>}
     , new apm_mix::output<double>{action<4>}
     , new apm_mix::output<double>{action<5>}
      ,new apm_mix::output<double>{action<6>}
     , new apm_mix::output<double>{action<7>}
     , new apm_mix::output<double>{action<8>}
     , new apm_mix::output<double>{action<9>}
   };
}

int main(int argc , char* argv[])
{
   if ( argc < 2){
      printf("Useage : %s <mixer_filename>\n",argv[0]);
      return EXIT_SUCCESS;
   }

   apm_mix::mixer_init(
      inputs, sizeof(inputs)/sizeof(inputs[0])
      ,outputs, sizeof(outputs)/sizeof(outputs[0])
   );

   bool mixer_build_success = false;
   if ( apm_lexer::open_file(argv[1])){
      if (apm_mix::mixer_create()){
         mixer_build_success = true;
         printf("mixer \"%s\" created OK!\n",argv[1]);
      }
      apm_lexer::close_file();
   }else{
      printf("open file %s failed\n",argv[1]);
   }
   
   if (mixer_build_success){
      printf("Press any key to start and once running press any key to quit\n");
      fflush(stdin);
      while (! key_was_pressed()){;}
      getchar(); // clear key pressed
      if ( open_joystick("/dev/input/js0")){
         while (get_joystick()->is_running() && ! key_was_pressed()){
            sleep_ms(20);
            apm_mix::mixer_eval();
         }
      }
   }
   close_joystick();
   printf("Quitting\n");
   return 0;
}

