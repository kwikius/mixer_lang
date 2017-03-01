#ifndef MIXER_LANG_PREDICATES_HPP_INCLUDED
#define MIXER_LANG_PREDICATES_HPP_INCLUDED

namespace apm_mix{

   struct abc_expr;

   bool is_bool_expr(abc_expr* ptr);

   bool are_boolean(abc_expr* lhs, abc_expr* rhs);

   bool is_float_expr(abc_expr* ptr);

   bool is_int_expr(abc_expr* ptr);

   bool is_numeric(abc_expr* ptr);

   bool are_numeric(abc_expr* lhs, abc_expr* rhs);

   bool are_same_type(abc_expr* lhs, abc_expr* rhs);

   bool are_same_numeric(abc_expr* lhs, abc_expr* rhs);

}
#endif // MIXER_LANG_PREDICATES_HPP_INCLUDED
