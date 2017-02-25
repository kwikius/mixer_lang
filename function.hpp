#ifndef MIXER_LANG_FUNCTION_HPP_INCLUDED
#define MIXER_LANG_FUNCTION_HPP_INCLUDED

namespace apm_mix{

   struct expr_list;

   struct abc_function_t{
      abc_function_t(int8_t num_args_in)
      : m_num_args{num_args_in}
      {}
     private: m_num_args
   }

   template <int NumArgs>
   struct function_t{
      function_t: abc_function_t{NumArgs}{}
   };

   struct function_call{
       int8_t get_num_args();
       
        
   };

}


#endif // MIXER_LANG_FUNCTION_HPP_INCLUDED
