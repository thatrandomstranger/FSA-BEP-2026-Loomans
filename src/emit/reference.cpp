#include "emit/reference.hpp"
#include "emit/indentation.hpp"

using namespace emit;

void Reference::emit(std::ostream& os) const {
  os << ref;
}
