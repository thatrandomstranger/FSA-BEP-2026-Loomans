#include <string>
#include <memory>
#include "emit/expression.hpp"

namespace emit {

struct MemberAccessExpression : Expression {
  std::shared_ptr<Expression> base;
  std::string member_name;

  MemberAccessExpression(std::shared_ptr<Expression> base, std::string member_name)
      : base(std::move(base)), member_name(std::move(member_name)) {}

  void emit(std::ostream& os) const override {
    os << base << ".\"" << member_name << "\"";
  }
};

}
