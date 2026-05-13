#pragma once
#include <sstream>
#include "emit/type.hpp"

namespace emit {

/// @brief Represents a variable declaration.
struct Variable {
  std::string name;
  std::shared_ptr<Type> type;

  void emit(std::ostream&) const;
};

}

std::ostream& operator<<(std::ostream&, const emit::Variable&);
