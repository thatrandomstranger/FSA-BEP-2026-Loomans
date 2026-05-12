#include "emit/struct_type.hpp"
#include "emit/indentation.hpp"
#include "emit/reference.hpp"
#include "emit/application.hpp"
#include "emit/member_access.hpp"
#include <assert.h>
#include <numeric>

using namespace emit;

void StructType::emit(std::ostream& os) const {
  assert(components.size() >= 1);
  os << indent << "TYPE " << name << " :\n";
  os << indent++ << "STRUCT\n";
  for (const auto& [n, t] : components)
    os << indent << n <<  " : " << t->name << ";\n";
  os << --indent << "END_STRUCT;\n";
  os << indent << "END_TYPE\n";
}

std::vector<int> StructType::get_indexing() const {
  std::vector<int> indexing;
  for (const auto& [_, type] : components) {
    auto sub_indexing = type->get_indexing_bounded();
    indexing.insert(indexing.end(), sub_indexing.begin(), sub_indexing.end());
  }
  return indexing;
}

std::vector<std::shared_ptr<Expression>> StructType::get_indexers(
    std::shared_ptr<Expression> expr) const {
  std::vector<std::shared_ptr<Expression>> indexers;
  for (const auto& [n, t] : components) {
    auto member = std::make_shared<MemberAccessExpression>(expr, n);
    auto sub_indexers = t->get_indexers(expr);
    indexers.insert(indexers.end(), sub_indexers.begin(), sub_indexers.end());
  }
  return indexers;
}
