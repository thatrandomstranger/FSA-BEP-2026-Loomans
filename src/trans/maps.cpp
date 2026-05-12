#include <map>
#include <set>
#include <format>
#include "translate.hpp"
#include "emit/binary.hpp"
#include "emit/selection.hpp"
#include "emit/reference.hpp"
#include "emit/assignment.hpp"


using namespace trans;


std::string get_typename(const mcrl2::data::sort_expression& exp, 
  const Context& context) {
  auto basic = mcrl2::data::basic_sort(exp);
  if (gctx.types.contains(basic.name()))
    return gctx.types.at(basic.name())->name;
  else
    return std::format("_unk_{}", std::string{basic.name()});
}

std::vector<emit::Function> trans::trans_maps(
  const mcrl2::lps::specification& spec,
  Context& context) {
  std::vector<emit::Function> ret;
  std::map<std::string, std::shared_ptr<emit::Selection>> selections;

  for (const auto& map : spec.data().user_defined_mappings()) {
    if (!mcrl2::data::is_function_sort(map.sort()))
      continue;
    auto sort = mcrl2::data::function_sort(map.sort());

    ret.push_back({
      .name = map.name(),
      .type = get_typename(sort.codomain(), context)
    });
    auto& func = ret.back();
    int index = 0;
    for (const auto& in : sort.domain())
      func.inputs.push_back({
        .name = std::format("in_{}", index++),
        .type = get_typename(in, context)
      });
    auto sel = std::make_shared<emit::Selection>();
    func.statements.push_back(sel);
    selections.insert_or_assign(map.name(), sel);
    context.symbs.insert_or_assign(map.name(), map.name());
  }

  for (const auto& eqn : spec.data().user_defined_equations()) {
    std::cout << eqn << std::endl;
    if (!mcrl2::data::is_application(eqn.lhs()))
      continue;

    auto appl = mcrl2::data::application(eqn.lhs());
    auto op = mcrl2::data::function_symbol(appl.head());
    auto cond = std::make_shared<emit::Binary>("&&");

    if (!selections.contains(op.name()))
      continue;


    auto eqn_context = context;
    for (int i = 0; i < appl.size(); i++) {
      if (mcrl2::data::is_variable(appl[i])) {
        auto var = mcrl2::data::variable(appl[i]);
        if (eqn_context.vars.contains(var.name()))
          cond->values.push_back(std::make_shared<emit::Binary>(
            "=", std::vector<std::shared_ptr<emit::Expression>>{
              std::make_shared<emit::Reference>(std::format("in_{}", i)),
              std::make_shared<emit::Reference>(eqn_context.vars.at(var.name()))
            }
          ));
        else
          eqn_context.vars.insert_or_assign(var.name(), std::format("in_{}", i));
      } else {
        cond->values.push_back(std::make_shared<emit::Binary>(
          "=", std::vector<std::shared_ptr<emit::Expression>>{
            std::make_shared<emit::Reference>(std::format("in_{}", i)),
            trans_expr(appl[i], eqn_context)
          }
        ));
      }
    }
    cond->values.push_back(trans_expr(eqn.condition(), eqn_context));

    auto& sel = *selections.at(op.name());
    auto statements = std::vector<std::shared_ptr<emit::Statement>>{
      std::make_shared<emit::Assignment>(
        op.name(),
        trans_expr(eqn.rhs(), eqn_context)
      )
    };
    sel.options.emplace_back(std::move(cond), std::move(statements));

  }

  return ret;
}
