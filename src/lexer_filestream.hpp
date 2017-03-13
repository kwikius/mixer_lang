#ifndef MIXER_LANG_LEXER_FILESTREAM_HPP_INCLUDED
#define MIXER_LANG_LEXER_FILESTREAM_HPP_INCLUDED

#include "lexer_stream.hpp"
#include "error.hpp"

namespace apm_lexer{

   struct filestream_t final : stream_t {
      filestream_t ( const char* const filename)
      : m_filename{filename},m_fileptr{nullptr}{}

      bool open() 
      {
         FILE* fp = ::fopen(m_filename, "r");
         if ( fp != nullptr){
            m_fileptr = fp;
            return true;
         }else{
            char buf[100];
            snprintf(buf,100,"open file \"%s\" failed",m_filename);
            apm_mix::yyerror(buf);
            return false;
         }
      }

      void close()
      {
         if ( m_fileptr != nullptr){
            ::fclose(m_fileptr);
            m_fileptr = nullptr;
         }
      }

      int get_char()
      {
         if (m_fileptr){
            return ::fgetc(m_fileptr);
         }else{ 
            return -1;
         }
      }

      int unget_char(int c)
      {
         return ::ungetc(c,m_fileptr);
      }

      ~filestream_t() 
      {
         close();
      }

   private:
      const char* const m_filename;
      FILE * m_fileptr;
   };

} //apm_lexer

#endif // MIXER_LANG_LEXER_FILESTREAM_HPP_INCLUDED
