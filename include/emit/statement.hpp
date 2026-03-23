#pragma once
#include <ostream>

namespace emit {

struct Statement {
  virtual void emit(std::ostream&) const = 0;

  virtual ~Statement() = default;
};

}

inline std::ostream& operator<<(std::ostream& os, const emit::Statement& st) {
  st.emit(os);
  return os;
}
