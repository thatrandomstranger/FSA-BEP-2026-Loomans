#pragma once
#include <sstream>

namespace emit {

/// @brief Represents a variable declaration.
struct Variable {
  std::string name;
  std::string type;

  void emit(std::ostream&) const;
};

}

std::ostream& operator<<(std::ostream&, const emit::Variable&);
