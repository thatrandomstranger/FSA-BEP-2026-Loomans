#include "emit/iterate.hpp"
#include "emit/indentation.hpp"
#include "emit/assignment.hpp"
#include "emit/reference.hpp"
#include <cassert>

using namespace emit;

void Iterate::emit(std::ostream& os) const {
  assert(dims.size() > 0);

  for (const auto& dim : dims) {
    os << indent++ << "FOR #" << dim.variable << " := 0 TO " << dim.size-1 << " DO\n";
    if (dim.expr) {
      auto a = std::make_shared<Assignment>(
          dim.expr, std::make_shared<emit::Reference>(dim.variable));
      os << *a;
    }
  }
  for (const auto& s : stmts) {
    os << *s;
  }
  for (const auto& dim : dims) {
    os << --indent << "END_FOR;\n";
  }
}
