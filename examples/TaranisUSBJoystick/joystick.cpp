
#include "joystick.hpp"

namespace util = mixer_lang_util;

namespace {
   util::joystick* p_joystick = nullptr;
}
util::joystick* get_joystick(){ return p_joystick;}

bool open_joystick(const char* device_name)
{
   try{
      p_joystick = new util::joystick(device_name);
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

namespace {

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
}

double get_pitch() { return get_joystick_value(idx::pitch);}
double get_yaw()  { return get_joystick_value(idx::yaw);}
double get_roll(){ return get_joystick_value(idx::roll);}
double get_throttle() { return get_joystick_value(idx::throttle);}
double get_flap() { return get_joystick_value(idx::flap);}
double get_control_mode() { return get_joystick_value(idx::control_mode);}

void sleep_ms(uint32_t ms)
{
  std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::milliseconds{ms});
}
