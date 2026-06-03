#include "translate.hpp"
#include "emit/reference.hpp"
#include "emit/selection.hpp"
#include "emit/assignment.hpp"

using namespace trans;

std::shared_ptr<emit::Reference> trans::get_ternary(
  const mcrl2::data::application& appl,
  const Context& context
) {
  assert(mcrl2::data::is_function_sort(appl.head().sort()));
  auto appl_sort = mcrl2::data::function_sort(appl.head().sort());
  assert(mcrl2::data::is_basic_sort(appl_sort.codomain()));
  auto dom_sort = mcrl2::data::basic_sort(appl_sort.codomain());
  auto type = gctx.types.at(dom_sort.name());
  auto psymbol = std::format("_ternary_{}", std::string(type->name));
  if (!gctx.symbs.contains(psymbol)) {
    emit::Function ternary = {
      .name = std::format("FC_Ternary_{}", type->name),
      .type = type,
      .inputs = {
        {"c", gctx.types.at("Bool")},
        {"e1", type},
        {"e0", type}
      }
    };

    auto sel = std::make_shared<emit::Selection>();
    sel->options.push_back(emit::Selection::Option{
      std::make_shared<emit::Reference>("#c"),
      std::vector<std::shared_ptr<emit::Statement>>{
        std::make_shared<emit::Assignment>(
          std::make_shared<emit::Reference>("#"+ternary.name),
          std::make_shared<emit::Reference>("#e1"))
      }
    });
    sel->else_stmts = {
      std::make_shared<emit::Assignment>(
          std::make_shared<emit::Reference>("#"+ternary.name),
          std::make_shared<emit::Reference>("#e0"))
    };

    gctx.symbs.insert_or_assign(psymbol, ternary.name);
    ternary.statements.push_back(std::move(sel));
    gctx.aux_functions.push_back(std::move(ternary));
  }

  return std::make_shared<emit::Reference>(
    gctx.symbs.at(psymbol)
  );
}
