#pragma once
#include "emit/type.hpp"

namespace emit
{

  struct ArrayType : Type
  {
    std::shared_ptr<Type> domain;
    std::shared_ptr<Type> range;

    ArrayType(std::string name,
              std::shared_ptr<Type> domain,
              std::shared_ptr<Type> range)
        : Type(std::move(name)),
          domain(std::move(domain)),
          range(std::move(range)) {}

    void emit(std::ostream &) const override;
  };

}
