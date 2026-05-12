#pragma once
#include "emit/type.hpp"

namespace emit {

struct EnumType : Type {
  std::vector<std::string> options;

  EnumType(std::string name, std::vector<std::string> options)
  : Type(std::move(name)), options(std::move(options)) {}

  void emit(std::ostream &) const override;

  std::vector<int> get_indexing() const override;

  std::vector<std::shared_ptr<Expression>> 
    get_indexers(std::shared_ptr<Expression>) const override;
};

}
