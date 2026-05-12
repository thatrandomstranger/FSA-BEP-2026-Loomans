#pragma once
#include <ostream>

namespace emit {

struct Expression {
  virtual void emit(std::ostream&) const = 0;

  virtual ~Expression() = default;
};

}

inline std::ostream& operator<<(std::ostream& os, const emit::Expression& ex) {
  ex.emit(os);
  return os;
}
