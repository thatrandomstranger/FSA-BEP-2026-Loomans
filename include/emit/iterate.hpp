#pragma once
#include "emit/statement.hpp"
#include "emit/expression.hpp"
#include <vector>
#include <memory>

namespace emit {

struct Iterate : Statement {
  struct Dimension {
    int size;
    std::string variable;
    std::shared_ptr<Expression> expr = {};
  };

  std::vector<Dimension> dims = {};
  std::vector<std::shared_ptr<emit::Statement>> stmts = {};

  explicit Iterate(std::vector<Dimension> dims = {},
                  std::vector<std::shared_ptr<emit::Statement>> stmts = {})
    : dims(std::move(dims)), stmts(std::move(stmts)) {}

  void emit(std::ostream&) const override;
};

}
