#include "translate.hpp"

using namespace trans;

emit::Type trans::trans_sort(const mcrl2::data::alias& alias) {
  auto name = std::string { alias.name().name() };
  auto options = std::vector<std::string> {};
  auto stru = mcrl2::data::structured_sort { alias.reference() };
  for (const auto& con : stru.constructors()) {
    assert(con.arguments().size() == 0 
      && "Constructors with arguments not implemented");
    options.push_back(con.name());
  }
  return {name, options};
}
