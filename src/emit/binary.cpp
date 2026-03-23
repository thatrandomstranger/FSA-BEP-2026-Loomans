#include "emit/binary.hpp"
#include <assert.h>

using namespace emit;

void Binary::emit(std::ostream& os) const {
  assert(values.size() >= 1);
  if (values.size() == 1) {
    os << *values[0];
    return;
  }
  os << '(' << *values[0];
  for (int i = 1; i < values.size(); i++) 
    os << ' ' << op << ' ' << *values[i];
  os << ')';
}
