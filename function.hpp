#ifndef MIXER_LANG_FUNCTION_HPP_INCLUDED
#define MIXER_LANG_FUNCTION_HPP_INCLUDED

namespace apm_mix{

    struct abc_expr;
    struct arg_list;

   struct function_builder {
      function_builder(){ }
      virtual ~ function_builder(){}
      virtual abc_expr * make_function(arg_list* args) = 0;
   };

}

#endif // MIXER_LANG_FUNCTION_HPP_INCLUDED
