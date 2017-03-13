#ifndef MIXER_LANG_LEXER_CSTRSTREAM_HPP_INCLUDED
#define MIXER_LANG_LEXER_CSTRSTREAM_HPP_INCLUDED

#include "lexer_stream.hpp"
#include "error.hpp"
#include <cstdint>

namespace apm_lexer{

   struct cstrstream_t final : stream_t {
      cstrstream_t ( const char* const cstring, uint16_t maxlen)
      : m_cstring{cstring},m_pos{0U},m_maxlen{maxlen}{}

      bool open() 
      {
         if (m_cstring != nullptr){
            m_pos = 0U;
            return true;
         }else{
            apm_mix::yyerror("null string in cstrstream");
            return false;
         }
      }

      void close(){}

      int get_char()
      {
         if ( m_pos == m_maxlen) { return -1;}
         int result = m_cstring[m_pos];
         if ( result != '\0'){
            ++m_pos;
            return result;
         }else{
            return -1;
         }
         
      }
      // note we cant chnge the char 
      int unget_char(int c)
      {
         if ( m_pos > 0) {
            --m_pos;
         }
         int result = m_cstring[m_pos];
         if ( result != '\0'){
            return result;
         }else{
            return -1;
         }
      }

      ~cstrstream_t() {}

   private:
      const char* const m_cstring;
      uint32_t m_pos;
      uint16_t const m_maxlen;
   };

} //apm_lexer

#endif // MIXER_LANG_LEXER_CSTRSTREAM_HPP_INCLUDED
