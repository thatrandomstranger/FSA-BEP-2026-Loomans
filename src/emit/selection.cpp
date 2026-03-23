#include "emit/selection.hpp"
#include "emit/indentation.hpp"

using namespace emit;

void Selection::emit(std::ostream& os) const {
  bool first = true;
  for (const auto& opt : options) {
    os << indent++ << (first ? "IF " : "ELSIF ") << *opt.condition << " THEN\n";
    for (const auto& st : opt.statements)
       os << *st;
    os << indent_down;
    first = false;
  }
  os << indent << "END_IF;\n";
}
