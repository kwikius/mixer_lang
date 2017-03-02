
#include "test.hpp"
#include "../function.hpp"
#include "../arg_list.hpp"
#include "../exprtree.hpp"

int errors = 0;

void args_test();
int main()
{
  args_test();
  EPILOGUE
}

using namespace apm_mix;

void args_test()
{
   abc_expr* expr1 = new constant<bool>{true};
   QUAN_CHECK(expr1->get_ID() == abc_expr::exprID::BOOL);
   QUAN_CHECK(expr1->is_constant());

   arg_list* args = nullptr;

   QUAN_CHECK(get_num_elements(args) == 0);
   QUAN_CHECK(get_arg(args,0) == nullptr);
   QUAN_CHECK(get_arg(args,100) == nullptr);

   args = new arg_list{expr1};
   QUAN_CHECK(get_num_elements(args) == 1);
   QUAN_CHECK(get_arg(args,0) == expr1);
   QUAN_CHECK(get_arg(args,1) == nullptr);
   QUAN_CHECK(get_arg(args,2) == nullptr);

   abc_expr* expr2 = new constant<double>{1000.0};

   auto * p = add_arg(args,expr2);
   QUAN_CHECK(p ==args);
   QUAN_CHECK(get_num_elements(args) == 2);
   QUAN_CHECK(get_arg(args,0) == expr1);
   QUAN_CHECK(get_arg(args,1) == expr2);
   QUAN_CHECK(get_arg(args,2) == nullptr);

   abc_expr* expr3 = new constant<int64_t>{100};

   auto * p1 = add_arg(args,expr3);
   QUAN_CHECK(p1 ==args);
   QUAN_CHECK(get_num_elements(args) == 3);
   QUAN_CHECK(get_arg(args,0) == expr1);
   QUAN_CHECK(get_arg(args,1) == expr2);
   QUAN_CHECK(get_arg(args,2) == expr3);
   QUAN_CHECK(get_arg(args,3) == nullptr);
   
   QUAN_CHECK(delete_args(args) == true);
}