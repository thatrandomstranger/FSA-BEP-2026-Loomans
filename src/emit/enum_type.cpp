#include "emit/enum_type.hpp"
#include "emit/indentation.hpp"
#include "emit/reference.hpp"
#include "emit/application.hpp"
#include <assert.h>

using namespace emit;

void EnumType::emit(std::ostream& os) const {
  assert(options.size() >= 1);
  os << indent << "TYPE " << name << " :\n";
  os << indent++ << "(\n";
  for (int i = 0; const auto& opt : options)
    os << indent << opt <<  (++i < options.size() ? ",\n" : "\n");
  os << --indent << ");\n";
  os << indent << "END_TYPE\n";
}

std::vector<int> EnumType::get_indexing() const {
  return {static_cast<int>(options.size())};
}

std::vector<std::shared_ptr<Expression>> EnumType::get_indexers(
    std::shared_ptr<Expression> e) const {
  static auto TO_INT = std::make_shared<Reference>("TO_INT");

  return {
    std::make_shared<Application>(TO_INT, std::vector{e})
  };
}
