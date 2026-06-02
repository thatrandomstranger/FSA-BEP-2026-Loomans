#pragma once
#include "emit/expression.hpp"
#include <memory>
#include <vector>

namespace emit {

struct Index : Expression {
  std::shared_ptr<Expression> array;
  std::vector<std::shared_ptr<Expression>> indexers;

  Index(std::shared_ptr<Expression> array, 
    std::vector<std::shared_ptr<Expression>> indexers={})
    : array(std::move(array)), indexers(std::move(indexers)) {}

  void emit(std::ostream &) const override;
};

}
