#pragma once
#include "emit/expression.hpp"
#include <memory>
#include <vector>

namespace emit {

struct FBCall : Expression {
  struct Argument {
    std::string name;
    std::shared_ptr<Expression> arg;
    bool is_input;
  };

  std::string fb;
  std::vector<Argument> parameters;

  FBCall(std::string fb, 
    std::vector<Argument> parameters={})
    : fb(std::move(fb)), parameters(std::move(parameters)) {}

  void emit(std::ostream &) const override;
};

}
