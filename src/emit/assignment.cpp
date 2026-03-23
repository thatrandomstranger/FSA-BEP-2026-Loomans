#include "emit/assignment.hpp"
#include "emit/indentation.hpp"

using namespace emit;

void Assignment::emit(std::ostream& os) const {
  os << indent << var << " := " << *expr << ";\n";
}
