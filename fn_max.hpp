#ifndef MIXER_LANG_FN_MAX_HPP_INCLUDED
#define MIXER_LANG_FN_MAX_HPP_INCLUDED

#include "function.hpp"

namespace apm_mix{
   struct fn_max : function_builder {
       abc_expr * make_function(arg_list* args) override;
   };

}


#endif // MIXER_LANG_FN_MAX_HPP_INCLUDED
