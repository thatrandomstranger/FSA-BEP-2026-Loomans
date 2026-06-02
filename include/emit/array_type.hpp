#pragma once
#include "emit/type.hpp"

namespace emit
{

  struct ArrayType : Type
  {
    std::shared_ptr<Type> domain;
    std::shared_ptr<Type> range;

    ArrayType(std::shared_ptr<Type> domain,
              std::shared_ptr<Type> range);

    void emit(std::ostream &) const override;
  };

}
