#pragma once
#include "emit/statement.hpp"
#include "emit/expression.hpp"
#include <memory>

namespace emit {

struct Assignment : Statement {
  std::shared_ptr<Expression> var;
  std::shared_ptr<Expression> expr;

  Assignment(std::shared_ptr<Expression> var, std::shared_ptr<Expression> expr)
    : var(std::move(var)), expr(std::move(expr)) {}

  void emit(std::ostream&) const override;
};

}
