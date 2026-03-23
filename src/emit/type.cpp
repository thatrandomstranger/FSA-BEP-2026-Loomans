#include "emit/type.hpp"
#include "emit/indentation.hpp"
#include <assert.h>

using namespace emit;

void Type::emit(std::ostream& os) const {
  assert(options.size() >= 1);
  os << indent << "TYPE " << name << ":\n";
  os << indent++ << "(\n";
  for (const auto& opt : options)
    os << indent << opt <<  ",\n";
  os << --indent << ");\n";
  os << indent << "END_TYPE\n";
}