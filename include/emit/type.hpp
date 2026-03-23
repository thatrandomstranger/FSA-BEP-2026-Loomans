#pragma once
#include <ostream>
#include <string>
#include <vector>

namespace emit {

struct Type {
  std::string name;
  std::vector<std::string> options;

  void emit(std::ostream&) const;
};

}

inline std::ostream& operator<<(std::ostream& os, const emit::Type& tp) {
  tp.emit(os);
  return os;
}