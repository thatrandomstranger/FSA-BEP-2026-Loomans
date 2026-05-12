#pragma once
#include "emit/type.hpp"

namespace emit {

struct RefType : Type {
  int size;

  RefType(std::string name, int size=-1) 
    : Type(std::move(name)), size(size) {}

  void emit(std::ostream& os) const override {}

  std::vector<int> get_indexing() const override {
    return {size};
  }

  std::vector<std::shared_ptr<Expression>> 
    get_indexers(std::shared_ptr<Expression> expr) const override {
    return {expr};
  }
};

}