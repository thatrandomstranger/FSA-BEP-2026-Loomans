#pragma once
#include "emit/expression.hpp"
#include <memory>
#include <vector>

namespace emit {

struct Application : Expression {
  std::shared_ptr<Expression> map;
  std::vector<std::shared_ptr<Expression>> parameters;

  Application(std::shared_ptr<Expression> map, 
    std::vector<std::shared_ptr<Expression>> parameters)
    : map(std::move(map)), parameters(std::move(parameters)) {}

  void emit(std::ostream &) const override;
};

}
