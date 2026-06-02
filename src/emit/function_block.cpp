#include "emit/function_block.hpp"
#include "emit/indentation.hpp"

using namespace emit;

void FunctionBlock::emit(std::ostream& os) const {
  os << indent++ << "FUNCTION_BLOCK " << name << "\n";
  
  if (inputs.size() > 0) {
    os << indent++ << "VAR_INPUT\n";
    for (const auto& var : inputs)
      os << var;
    os << --indent << "END_VAR\n\n";
  }

  if (outputs.size() > 0) {
    os << indent++ << "VAR_OUTPUT\n";
    for (const auto& var : outputs)
      os << var;
    os << --indent << "END_VAR\n\n";
  }

  if (variables.size() > 0) {
    os << indent++ << "VAR\n";
    for (const auto& var : variables)
      os << var;
    os << --indent << "END_VAR\n\n";
  }
  
  os << indent << "BEGIN\n";

  for (const auto& st : statements)
    os << *st;

  os << --indent << "END_FUNCTION_BLOCK\n";
}

std::ostream& operator<<(std::ostream& os, const FunctionBlock& func) {
  func.emit(os);
  return os;
}
