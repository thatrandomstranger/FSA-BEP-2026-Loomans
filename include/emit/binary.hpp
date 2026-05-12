#pragma once
#include "emit/expression.hpp"
#include <memory>
#include <vector>

namespace emit
{

struct Binary : Expression {
  std::string op;
  std::vector<std::shared_ptr<Expression>> values;

  Binary(std::string op, std::vector<std::shared_ptr<Expression>> values={}) 
          : op(std::move(op)), values(std::move(values)) {}

  void emit(std::ostream &) const override;
};

}
