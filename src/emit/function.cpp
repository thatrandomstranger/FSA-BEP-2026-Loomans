#include "emit/function.hpp"
#include "emit/indentation.hpp"

using namespace emit;

void Function::emit(std::ostream& os) const {
  os << indent++ << "FUNCTION " << name << " : " << (io ? "Void" : type->name) << "\n";
  if (io) {
    os << indent++ << "VAR_IN_OUT\n";
    os << Variable("in_0", type);
    os << --indent << "END_VAR\n\n";
  }
  
  os << indent++ << "VAR_INPUT\n";
  for (const auto& var : inputs)
    os << var;
  os << --indent << "END_VAR\n\n";

  os << indent << "BEGIN\n";

  for (const auto& st : statements)
    os << *st;

  os << --indent << "END_FUNCTION\n";
}

std::ostream& operator<<(std::ostream& os, const Function& func) {
  func.emit(os);
  return os;
}
