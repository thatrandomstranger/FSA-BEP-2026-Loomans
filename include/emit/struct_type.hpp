#pragma once
#include "emit/type.hpp"

namespace emit {

struct StructType : Type {
  using Component = std::pair<std::string, std::shared_ptr<Type>>;
  using Components = std::vector<Component>;

  Components components;

  StructType(std::string name, Components components = {})
  : Type(std::move(name)), components(std::move(components)) {}

  void emit(std::ostream &) const override;

  std::vector<int> get_indexing() const override;

  std::vector<std::shared_ptr<Expression>> 
    get_indexers(std::shared_ptr<Expression>) const override;
};

}
