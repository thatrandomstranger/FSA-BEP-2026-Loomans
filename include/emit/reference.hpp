#pragma once
#include "emit/expression.hpp"

namespace emit {

struct Reference : Expression {
  std::string ref;

  explicit Reference(std::string ref) : ref(ref) {}

  void emit(std::ostream&) const override;
};

}
