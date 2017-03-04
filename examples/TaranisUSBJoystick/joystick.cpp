
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

   apm_mix::float_t get_joystick_value(uint8_t n){
      if ( p_joystick){
         return static_cast<apm_mix::float_t>(p_joystick->get_channel(n) / 32767.0);
      }else{
         return static_cast<apm_mix::float_t>(0.0);
      }
   }
}

apm_mix::float_t get_pitch() { return get_joystick_value(idx::pitch);}
apm_mix::float_t get_yaw()  { return get_joystick_value(idx::yaw);}
apm_mix::float_t get_roll(){ return get_joystick_value(idx::roll);}
apm_mix::float_t get_throttle() { return get_joystick_value(idx::throttle);}
apm_mix::float_t get_flap() { return get_joystick_value(idx::flap);}
apm_mix::float_t get_control_mode() { return get_joystick_value(idx::control_mode);}

void sleep_ms(uint32_t ms)
{
   std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::milliseconds{ms});
}
