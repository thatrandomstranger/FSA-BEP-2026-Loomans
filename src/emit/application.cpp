#include "emit/application.hpp"
#include <assert.h>

using namespace emit;

void Application::emit(std::ostream& os) const {
  if (parameters.size() == 0) {
    os << *map << "()";
    return;
  }
  os << *map << '(' << *parameters[0];
  for (int i = 1; i < parameters.size(); i++) 
    os << ", " << *parameters[i];
  os << ')';
}
