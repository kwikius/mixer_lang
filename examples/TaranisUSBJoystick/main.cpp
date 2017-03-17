
#include <cstdio>
#include <mixer_lang.hpp>
#include <mixer_lang_filestream.hpp>
#include "joystick.hpp"

namespace {

   // This value simulates the airspeed sensor reading
   // TODO make a way to vary it when running for sim ... prob
   // more suitable for a GUI version
   apm_mix::float_t airspeed_m_per_s = 0.0; //

   // true simulates a possible sensor failure 
   // which may mean (for example) airspeed reading is no good
   bool in_failsafe = false;  //
   
   // The mixer uses functions pointers  to get its inputs
   // The inputs can be of type Bool, Integer or Float 
   apm_mix::float_t get_airspeed(){ return airspeed_m_per_s;}
   bool failsafe_on() { return in_failsafe;}


   // The input array represents all the available inputs.
   // The inputs are passed to the mixer constructor
   // TODO make inputs optionally constant
   // TODO add integer index inputs option
   // if the function is are unambiguous, the type of the input is deduced from the signature of the function
   // the function signatures below are declared to return a float type in joystick.hpp
   // you can also use anexplicit cast as in the lambda functions
   // c++ lambda functions can also be used as shown in the array
   // you can use different input types as shown by the bool and int inputs at the end of the array
   apm_mix::input_pair 
   inputs[] = { 
       {"Pitch", get_pitch}
      ,{"Yaw",  get_yaw}
      ,{"Roll", get_roll}
      ,{"Throttle", get_throttle}
      ,{"Flap", get_flap}
      ,{"Airspeed", get_airspeed}
      ,{"ControlMode", get_control_mode}
      ,{"ARSPD_MIN", static_cast<apm_mix::float_t(*)()>([]()->apm_mix::float_t{return 10.0;})}
      ,{"ARSPD_CRUISE",static_cast<apm_mix::float_t(*)()>([]()->apm_mix::float_t{return 12.0;})}
      ,{"ARSPD_MAX", static_cast<apm_mix::float_t(*)()>([]()->apm_mix::float_t{return 20.0;})}
      ,{"FAILSAFE_ON", failsafe_on}
      ,{"DUMMY_INT", static_cast<apm_mix::int_t(*)()>([]()->apm_mix::int_t{return 1000;})}
   };

   // The mixers also uses function pointers to send its outputs.
   // outputs can also be of type Bool, Integer or Float
   // Here for simplicity, we just pass an output function 'output_action'
   // that prints the output to stdout
   // The non type template parameter is handy to provide the index
   template<unsigned N>
   void output_action(apm_mix::float_t const & v)
   {
      if ( v == static_cast<apm_mix::float_t>(0)){
         printf("output[%u] = % 6.3f\n",N,static_cast<double>(v));
      }else{
         printf("output[%u] = %+6.3f\n",N,static_cast<double>(v));
      }
   }

   apm_mix::output<apm_mix::float_t> 
   outputs[] = {
      {output_action<0>}
     ,{output_action<1>}
     ,{output_action<2>}
     ,{output_action<3>}
     ,{output_action<4>}
     ,{output_action<5>}
     ,{output_action<6>}
     ,{output_action<7>}
   };
}

int main(int argc , char* argv[])
{
   if ( argc < 2){
      printf("Useage : %s <mixer_filename>\n",argv[0]);
      return EXIT_SUCCESS;
   }

   // create the mixer from a filestream
   apm_lexer::filestream_t stream{argv[1]};

   if ( apm_mix::mixer_create(
          &stream
         ,inputs, sizeof(inputs)/sizeof(inputs[0])
         ,outputs, sizeof(outputs)/sizeof(outputs[0])
      )){
      printf ("\n\n#########################################################\n");
      printf     ("#                                                       #\n");
      printf     ("#              Welcome to mixer_lang                    #\n");
      printf     ("#                                                       #\n");
      printf     ("#########################################################\n\n");
      printf("mixer \"%s\" was created ...  OK!\n\n",argv[1]);
   }else{
      printf("create mixer from %s failed ... quitting\n",argv[1]);
      return EXIT_FAILURE;
   }

   printf("Switch on your Taranis and once on, then connect it to your PC via USB\n\n");
   printf("Then when ready,press any key to start and once running press any key to quit\n\n");
   fflush(stdin);
   while (! key_was_pressed()){;} 

   getchar(); // clear key pressed
   auto result = EXIT_FAILURE; // main return value
   if ( open_joystick("/dev/input/js0")){
      while (get_joystick()->is_running() && ! key_was_pressed()){
         sleep_ms(20);
         apm_mix::mixer_eval();
      }
      close_joystick();
      result = EXIT_SUCCESS;
   }
   apm_mix::close_mixer();
   return result;
}
