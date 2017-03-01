
#include <cstdio>
#include "mixer.hpp"
#include "lexer.hpp"
#include <quan/utility/timer.hpp>
#include <quan/joystick.hpp>
#include <quan/key_was_pressed.hpp>

apm_mix::mixer_t* fn_mix();

namespace {

   double airspeed = 0.0; // test pathological on airspeed based mixer
   bool in_failsafe = false;  //

   double get_pitch();
   double get_yaw();
   double get_roll();
   double get_throttle();
   double get_airspeed(){ return airspeed;}
   double get_flap();
   double get_control_mode();
   bool open_joystick(const  char * device_name);
   quan::joystick* get_joystick();
   void close_joystick();
   void sleep_ms(uint32_t ms)
   {
     std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::milliseconds{ms});
   }
}
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

int main(int argc , char* argv[])
{
   if ( argc < 2){
      printf("Useage : %s <mixer_filename>\n",argv[0]);
      return EXIT_SUCCESS;
   }

   apm_mix::mixer_init(inputs, sizeof(inputs)/sizeof(inputs[0]));

   bool mixer_build_success = false;
   if ( apm_lexer::open_file(argv[1])){
      // fn_mix builds the mixer from the input stream
      if (fn_mix()){
        // if the build is succesfull
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
      while (! quan::key_was_pressed()){;}
      getchar(); // clear key pressed
      if ( open_joystick("/dev/input/js0")){
         while (get_joystick()->is_running() && ! quan::key_was_pressed()){
            sleep_ms(20);
            apm_mix::eval_mixer_outputs();
         }
      }
   }
   close_joystick();
   printf("Quitting\n");
   return 0;
}

namespace {

   quan::joystick* p_joystick = nullptr;

   quan::joystick* get_joystick(){ return p_joystick;}

   bool open_joystick(const char* device_name)
   {
      try{
         p_joystick = new quan::joystick(device_name);
         return p_joystick != nullptr;
      }catch (std::exception & e){
         printf("Exception ; %s\n", e.what());
         return false;
      }
   }

   void close_joystick() 
   {
      delete p_joystick;
      p_joystick = nullptr;
   }

   struct idx{
      static constexpr uint8_t roll = 0;
      static constexpr uint8_t pitch = 1;
      static constexpr uint8_t throttle = 2;
      static constexpr uint8_t yaw = 3;
      static constexpr uint8_t flap = 4;
      static constexpr uint8_t control_mode = 5;
   };
   
   double get_joystick_value(uint8_t n){
      if ( p_joystick){
         return static_cast<double>(p_joystick->get_channel(n)) / 32767.0;
      }else{
         return 0.0;
      }
   }

   double get_pitch() { return get_joystick_value(idx::pitch);}
   double get_yaw()  { return get_joystick_value(idx::yaw);}
   double get_roll(){ return get_joystick_value(idx::roll);}
   double get_throttle() { return get_joystick_value(idx::throttle);}
   double get_flap() { return get_joystick_value(idx::flap);}
   double get_control_mode() { return get_joystick_value(idx::control_mode);}
}
