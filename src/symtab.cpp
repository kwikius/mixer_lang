#ifndef SYMTAB_CPP_INCLUDED
#define SYMTAB_CPP_INCLUDED

#include "abc_expr.hpp"
#include "lookup.cpp"

template struct apm_mix::lookup_t<apm_mix::abc_expr*>;

#endif // SYMTAB_CPP_INCLUDED
