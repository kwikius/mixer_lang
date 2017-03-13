#ifndef MIXER_LANG_LEXER_STREAM_HPP_INCLUDED
#define MIXER_LANG_LEXER_STREAM_HPP_INCLUDED

/*
 abc stream abstraction
*/

namespace apm_lexer{

   struct stream_t {
      virtual ~stream_t(){}
      virtual int get_char() = 0;
      virtual int unget_char(int c) = 0;
      virtual bool open()  = 0;
      virtual void close() = 0;
   protected:
      stream_t(){};
   private:
      stream_t (stream_t const &) = delete;
      stream_t & operator = (stream_t const &) = delete;
   };

} //apm_lexer

#endif // MIXER_LANG_LEXER_STREAM_HPP_INCLUDED

