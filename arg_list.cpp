
#include "abc_expr.hpp"
#include "arg_list.hpp"

apm_mix::arg_list* apm_mix::add_arg(arg_list* args, abc_expr* expr)
{
   if ( args == nullptr){
      return new arg_list{expr};
   }else{
      auto * temp = args;
      while ( temp->m_next != nullptr){
         temp = temp->m_next;
      }
      temp->m_next = new arg_list{expr};
      return args;
   }
}

int32_t apm_mix::get_num_elements(arg_list const* args)
{
   int arg_count = 0;
   while ( args != nullptr){
      args = args->m_next;
      ++arg_count;
   }
   return arg_count;     
}

bool apm_mix::delete_args(apm_mix::arg_list* args)
{
   while (args != nullptr){
      auto * temp = args;
      args = args->m_next;
      delete temp;
   }
   return true;
}
  
apm_mix::abc_expr * apm_mix::get_arg(arg_list const* args, uint32_t i)
{
   uint32_t arg_count = 0;
   while ( args != nullptr){
      if (arg_count == i){
         return args->m_arg;
      }else{
         args = args->m_next;
         ++arg_count;
      }
   }
   return nullptr;
}
