#ifndef MIXER_LANG_FUNCTION_HPP_INCLUDED
#define MIXER_LANG_FUNCTION_HPP_INCLUDED

namespace apm_mix{

    struct abc_expr;
    struct arg_list;

    typedef abc_expr* (*function_builder)(arg_list* args);

    abc_expr * make_function_max(arg_list* args);
    abc_expr * make_function_min(arg_list* args);
    abc_expr * make_function_if(arg_list* args);

}

#endif // MIXER_LANG_FUNCTION_HPP_INCLUDED
