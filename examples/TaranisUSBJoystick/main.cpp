
#include <cstdio>
#include "../../src/mixer.hpp"
#include "joystick.hpp"

namespace {

   // This value simulates the airspeed sensor reading
   // TODO make a way to vary it when running for sim ... prob
   // more suitable for a GUI version
   apm_mix::float_t airspeed_m_per_s = 0.0; //

   // true simulates a possible sensor failure 
   // which may mean airspeed reading is no good
   bool in_failsafe = false;  //

   // The mixer uses functions pointers  to get its inputs
   // The inputs can be of type Bool, Integer or Float 
   apm_mix::float_t get_airspeed(){ return airspeed_m_per_s;}


   // The input array represents all the available inputs.
   // The inputs are passed to the mixer constructor
   // TODO make inputs optionally constant
   // TODO add integer index inputs option

   apm_mix::input_pair inputs[] = { 
      // if the function is are unambiguous, the type of the input is deduced from the signature of the function
      // the function signatures below are declared to return a float type in joystick.hpp
      // you can also use anexplicit cast as in the lambda functions
      apm_mix::input_pair{"Pitch", get_pitch},
      apm_mix::input_pair{"Yaw",  get_yaw},
      apm_mix::input_pair{"Roll", get_roll},
      apm_mix::input_pair{"Throttle", get_throttle},
      apm_mix::input_pair{"Flap", get_flap},
      apm_mix::input_pair{"Airspeed", get_airspeed},
      apm_mix::input_pair{"ControlMode", get_control_mode},
      // c++ lambda functions can also be used
      apm_mix::input_pair{"ARSPD_MIN", static_cast<apm_mix::float_t(*)()>([]()->apm_mix::float_t{return 10.0;})},
      apm_mix::input_pair{"ARSPD_CRUISE",static_cast<apm_mix::float_t(*)()>([]()->apm_mix::float_t{return 12.0;})},
      apm_mix::input_pair{"ARSPD_MAX", static_cast<apm_mix::float_t(*)()>([]()->apm_mix::float_t{return 20.0;})},
      // for completeness a bool input
      apm_mix::input_pair{"FAILSAFE_ON", static_cast<bool(*)()>([]()->bool{return in_failsafe;})},
      // for completeness an int input
      apm_mix::input_pair{"DUMMY_INT", static_cast<apm_mix::int_t(*)()>([]()->apm_mix::int_t{return 1000;})}
   };

   // The mixers also uses function pointers to send its outputs.
   // outputs can also be of type Bool, Integer or Float
   // Here for simplicity, we just pass an output function 'action'
   // that prints the output to stdout
   // The non type template parameter is handy to provide the index
   template<unsigned N>
   void action(apm_mix::float_t const & v)
   {
      printf("output[%u] = %f\n",N,static_cast<double>(v));
   }

   // to show that int_t can be assigned as an output function also
   template<unsigned N>
   void action(apm_mix::int_t const & v)
   {
      printf("output[%u] = %d\n",N, static_cast<int>(v));
   }

   // to show that bool can be assigned as an output function also
   template<unsigned N>
   void action(bool const & v)
   {
      const char * const true_ = "true";
      const char* const false_ = "false";
      printf("output[%u] = %s\n",N, (v? true_:false_));
   }

   // Outputs are passed as an array to the mixer constructor
   // Outputs can output a type of Integer, Bool or Float
   // Allowing different output types may be overkill
   // The types should probably be the same ( homogeneous)
   // then you can switch physical rc output channels without issues.
   // maybe though you want to set some channels true or false
   // e.g leds indicators camera switches
   // integer types can represent pwm exactly etc
   // TBD
   apm_mix::abc_expr* outputs[]
   = {
      
     , new apm_mix::output<apm_mix::float_t>{action<0>}
     , new apm_mix::output<apm_mix::float_t>{action<1>}
     , new apm_mix::output<apm_mix::float_t>{action<2>}
     , new apm_mix::output<apm_mix::float_t>{action<3>}
     , new apm_mix::output<apm_mix::float_t>{action<4>}
     , new apm_mix::output<apm_mix::float_t>{action<5>}
      ,new apm_mix::output<apm_mix::float_t>{action<6>}
     , new apm_mix::output<apm_mix::float_t>{action<7>}
     , new apm_mix::output<apm_mix::float_t>{action<8>}
     , new apm_mix::output<apm_mix::int_t>{action<9>}  // to demonstrate that these types can also be used
     , new apm_mix::output<bool>{action<10>}
   };
}

int main(int argc , char* argv[])
{
   if ( argc < 2){
      printf("Useage : %s <mixer_filename>\n",argv[0]);
      return EXIT_SUCCESS;
   }

   if ( apm_mix::mixer_create(
         argv[1]
         ,inputs, sizeof(inputs)/sizeof(inputs[0])
         ,outputs, sizeof(outputs)/sizeof(outputs[0])
      )){
         printf("mixer \"%s\" created OK!\n",argv[1]);
   }else{
      printf("create mixer from %s failed ... quitting\n",argv[1]);
      return EXIT_FAILURE;
   }
   
   printf("Press any key to start and once running press any key to quit\n");
   fflush(stdin);
   while (! key_was_pressed()){;}
   getchar(); // clear key pressed
   if ( open_joystick("/dev/input/js0")){
      while (get_joystick()->is_running() && ! key_was_pressed()){
         sleep_ms(20);
         apm_mix::mixer_eval();
      }
      close_joystick();
   }
}
