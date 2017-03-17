

#include <mixer_lang.hpp>
#include <cstdlib>
#include <cstring>

char * apm_mix::duplicate_string(const char *s)
{
    return ::strdup(s);
}

void apm_mix::delete_string(const char* s)
{
    ::free(const_cast<char*>(s));
}