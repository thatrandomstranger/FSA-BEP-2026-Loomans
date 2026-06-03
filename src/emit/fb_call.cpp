#include "emit/fb_call.hpp"
#include "emit/indentation.hpp"
#include <assert.h>

using namespace emit;

void FBCall::emit(std::ostream &os) const
{
  if (parameters.size() == 0)
  {
    os << fb << "()";
    return;
  }
  os << fb << '('
     << parameters[0].name
     << (parameters[0].is_input ? " := " : " => ")
     << *parameters[0].arg;
  for (int i = 1; i < parameters.size(); i++)
    os << ", " << parameters[i].name
       << (parameters[i].is_input ? " := " : " => ")
       << *parameters[i].arg;
  os << ")";
}
