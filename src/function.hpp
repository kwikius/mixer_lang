#ifndef MIXER_LANG_FUNCTION_HPP_INCLUDED
#define MIXER_LANG_FUNCTION_HPP_INCLUDED

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

namespace apm_mix{

    struct abc_expr;
    struct arg_list;

    typedef abc_expr* (*function_builder)(arg_list* args);

    abc_expr * make_function_max(arg_list* args);
    abc_expr * make_function_min(arg_list* args);
    abc_expr * make_function_if(arg_list* args);

}

#endif // MIXER_LANG_FUNCTION_HPP_INCLUDED
