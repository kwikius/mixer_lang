#ifndef MIXER_LANG_UTIL_JOYSTICK_HPP_INCLUDED
#define MIXER_LANG_UTIL_JOYSTICK_HPP_INCLUDED

#include <cstdint>
#include <cstdlib>
#include <cstdio>

#include <unistd.h>
#include <fcntl.h>
#include <linux/joystick.h>
#include <stdexcept>
#include <thread>    
#include <chrono> 
#include <atomic> 

/*
  link with pthread
*/

// n.b namespace mixer_lang_util aliased to namespace util in examples (brevity v name collisions)
namespace mixer_lang_util{

   struct joystick{
      static constexpr uint8_t num_channels = 8;
      
      joystick(const char* device_name) // e.g device_name = "/dev/input/js0"
      : m_fd{open(device_name, O_RDONLY | O_NONBLOCK)}, m_thread_running{false}, m_want_thread_quit{true}
      {
         if (m_fd < 0){
            throw std::runtime_error("failed to open joystick device");
         }
         for (auto & v : m_channel){v = 0;}
         m_thread_running = true;
         m_want_thread_quit = false;
         m_update_thread = std::thread{std::bind(&joystick::update,this)};
      }
      ~joystick(){ end_thread(); if (m_fd >= 0) { close(m_fd);} }

      bool is_running() const { return m_thread_running == true;}

      int32_t get_channel(uint8_t channel)const
      {
         if ( channel < num_channels){
            return m_channel[channel];
         }else{
            return 0;
         }
      }

      void end_thread()
      {
         m_want_thread_quit = true;
         while (m_thread_running){;}
         m_update_thread.join();
      }

   private:
      void update()
      {
         for(;;){
            while (read (m_fd, &m_event, sizeof(m_event)) == sizeof(m_event)) {
               if ((m_event.type & JS_EVENT_AXIS)==JS_EVENT_AXIS){
                  set_channel();
               }
            }
            if( (errno != EAGAIN) || m_want_thread_quit) {
               if ( errno != EAGAIN){
                  printf("joystick device read failed\n");
               }
               m_thread_running = false;
               return;
            }
            std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::milliseconds{10});
         }
      }
     
      void set_channel()
      {
         if ( m_event.number < num_channels){
            m_channel[m_event.number] = m_event.value;
         }
      }
      std::atomic<int32_t>   m_channel[num_channels];
      std::atomic<int>       m_fd;
      std::atomic<bool>      m_thread_running;
      std::atomic<bool>      m_want_thread_quit;
      js_event               m_event;
      std::thread            m_update_thread;
      joystick (const joystick &)= delete;
      joystick & operator = (const joystick &)= delete;
   };

}  // mixer_lang_util

bool open_joystick(const  char * device_name);
mixer_lang_util::joystick* get_joystick();
void close_joystick();
double get_pitch();
double get_yaw();
double get_roll();
double get_throttle();
double get_flap();
double get_control_mode();
void sleep_ms(uint32_t ms);
bool key_was_pressed();

#endif // MIXER_LANG_UTIL_JOYSTICK_HPP_INCLUDED
