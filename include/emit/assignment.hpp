#pragma once
#include "emit/statement.hpp"
#include "emit/expression.hpp"
#include <memory>

namespace emit {

struct Assignment : Statement {
  std::string var;
  std::shared_ptr<Expression> expr;

  Assignment(std::string var, std::shared_ptr<Expression> expr)
    : var(std::move(var)), expr(std::move(expr)) {}

  void emit(std::ostream&) const override;
};

}
