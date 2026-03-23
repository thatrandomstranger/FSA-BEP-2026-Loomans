#pragma once
#include "emit/statement.hpp"
#include "emit/expression.hpp"
#include <vector>
#include <memory>

namespace emit {

struct Selection : Statement {
  struct Option {
    std::shared_ptr<Expression> condition;
    std::vector<std::shared_ptr<Statement>> statements;

    Option(std::shared_ptr<Expression> condition, 
           std::vector<std::shared_ptr<Statement>> statements = {})
           : condition(std::move(condition)), 
             statements(std::move(statements)) {}
  };

  std::vector<Option> options;

  explicit Selection(std::vector<Option> options = {})
    : options(std::move(options)) {}

  void emit(std::ostream&) const override;
};

}
