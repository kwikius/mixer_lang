#ifndef MIXER_LANG_CONVERSION_BASIC_CHAR_PTR_CONVERTER_HPP_INCLUDED
#define MIXER_LANG_CONVERSION_BASIC_CHAR_PTR_CONVERTER_HPP_INCLUDED

/*
 Copyright (c) 2017 Andy Little 

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>
*/

#ifndef __AVR__
#include <cstdint>
#else
#include <stdint.h>
#endif

/*
   conversion from a string to some type
   used as a base class for int and float converters
*/

namespace apm_mix{ namespace util{

   struct basic_char_ptr_converter{

       int get_errno()const {return m_conversion_error;}
       uint32_t get_parse_length() const { return m_parse_length;}

   protected:
      basic_char_ptr_converter():m_conversion_error{0}, m_parse_length{0}{}

      void reset()
      {
         m_conversion_error = 0;
         m_parse_length = 0;
      }

      static int atoi(char ch)
      {
         return ch - '0';
      }

      static int32_t strnlen( const char* str, int32_t maxlen)
      {
         if ( str){
            for (int32_t i =0; i < maxlen; ++i){
               if ( str[i] =='\0'){
                  return i;
                }
            }
         }
         return -1;
      }

      int m_conversion_error;
      uint32_t  m_parse_length;
   };

}} // apm_mix::util

#endif // MIXER_LANG_CONVERSION_BASIC_CHAR_PTR_CONVERTER_HPP_INCLUDED
