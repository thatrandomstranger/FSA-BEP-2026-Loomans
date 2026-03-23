#include "emit/variable.hpp"
#include "emit/indentation.hpp"

using namespace emit;

void Variable::emit(std::ostream& out) const {
  out << indent << name << "\t:\t" << type << ";\n";
}

std::ostream& operator<<(std::ostream& os, const Variable& var) {
  var.emit(os);
  return os;
}
