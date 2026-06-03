#include <map>
#include <set>
#include <format>
#include "translate.hpp"
#include "emit/binary.hpp"
#include "emit/selection.hpp"
#include "emit/reference.hpp"
#include "emit/assignment.hpp"
#include "emit/reference_type.hpp"
#include "emit/array_type.hpp"
#include "emit/index.hpp"

using namespace trans;

std::shared_ptr<emit::Type> get_type(const mcrl2::data::sort_expression &exp,
                                     const Context &context)
{
  auto basic = mcrl2::data::basic_sort(exp);
  if (gctx.types.contains(basic.name()))
    return gctx.types.at(basic.name());
  else
    return std::make_shared<emit::RefType>(
        std::format("_unk_{}", std::string{basic.name()}));
}

std::vector<emit::Function> trans::trans_maps(
    const mcrl2::lps::specification &spec,
    const Context &context)
{
  std::vector<emit::Function> ret;
  std::map<std::string, std::shared_ptr<emit::Selection>> selections;
  std::map<std::string, std::shared_ptr<emit::Type>> io_types;
  std::map<std::string, std::vector<emit::Variable>> variables;
  std::map<std::string, int> ret_indices;

  for (const auto &map : spec.data().user_defined_mappings())
  {
    if (!mcrl2::data::is_function_sort(map.sort()))
    {
      gctx.constants.insert_or_assign(map.name(),
                                      std::shared_ptr<emit::Expression>{});
      continue;
    }
    auto sort = mcrl2::data::function_sort(map.sort());

    ret.push_back({.name = std::format("FC_Gen_{}", std::string(map.name())),
                   .type = get_type(sort.codomain(), context)});
    int index = 0;

    for (const auto &in : sort.domain())
      ret.back().inputs.push_back({.name = std::format("in_{}", index++),
                                   .type = get_type(in, context)});
    auto sel = std::make_shared<emit::Selection>();
    ret.back().statements.push_back(sel);
    selections.insert_or_assign(map.name(), sel);
    gctx.symbs.insert_or_assign(map.name(), ret.back().name);
    variables.insert_or_assign(map.name(), ret.back().inputs);
    ret_indices.insert_or_assign(map.name(), ret.size() - 1);

    if (emit::ArrayType *at;
        ret.back().inputs.size() >= 1 && (at = dynamic_cast<emit::ArrayType *>(ret.back().inputs[0].type.get())) && (ret.back().type->name == at->name))
    {
      ret.back().io = true;
      ret.back().inputs.erase(ret.back().inputs.begin());
      io_types.insert_or_assign(std::string(map.name()), ret.back().inputs[0].type);
    }

    gctx.func_symbs.insert_or_assign(map.name(), ret.back());
  }

  for (const auto &eqn : spec.data().user_defined_equations())
  {
    if (!mcrl2::data::is_application(eqn.lhs()))
    {
      auto con = mcrl2::data::function_symbol(eqn.lhs());
      std::vector<std::shared_ptr<emit::Statement>> _aux_stmts;
      std::vector<emit::Variable> _aux_vars;
      gctx.constants.insert_or_assign(con.name(), trans_expr(eqn.rhs(), context, _aux_stmts, _aux_vars));
      continue;
    }

    auto appl = mcrl2::data::application(eqn.lhs());
    auto op = mcrl2::data::function_symbol(appl.head());
    auto cond = std::make_shared<emit::Binary>("&&");

    if (!selections.contains(op.name()))
    {
      std::vector<std::shared_ptr<emit::Statement>> _aux_stmts;
      std::vector<emit::Variable> _aux_vars;
      gctx.constants.insert_or_assign(op.name(), trans_expr(eqn.rhs(), context, _aux_stmts, _aux_vars));
      continue;
    }

    auto eqn_context = context;
    std::vector<std::shared_ptr<emit::Statement>> cond_aux_stmts;
    std::vector<std::shared_ptr<emit::Statement>> aux_stmts;
    std::vector<emit::Variable> aux_vars;

    for (int i = 0; i < appl.size(); i++)
    {
      if (mcrl2::data::is_variable(appl[i]))
      {
        auto var = mcrl2::data::variable(appl[i]);
        if (eqn_context.vars.contains(var.name()))
          cond->values.push_back(std::make_shared<emit::Binary>(
              "=", std::vector<std::shared_ptr<emit::Expression>>{
                       std::make_shared<emit::Reference>(std::format("in_{}", i)),
                       std::make_shared<emit::Reference>("#" + eqn_context.vars.at(var.name()).name)}));
        else
          eqn_context.vars.insert_or_assign(var.name(), variables.at(op.name())[i]);
      }
      else
      {
        cond->values.push_back(std::make_shared<emit::Binary>(
            "=", std::vector<std::shared_ptr<emit::Expression>>{
                     std::make_shared<emit::Reference>(std::format("in_{}", i)),
                     trans_expr(appl[i], eqn_context, cond_aux_stmts, aux_vars)}));
      }
    }

    cond->values.push_back(trans_expr(eqn.condition(), eqn_context, cond_aux_stmts, aux_vars));

    auto &sel = *selections.at(op.name());
    auto statements = std::vector<std::shared_ptr<emit::Statement>>{};
    if (io_types.contains(op.name()))
    {
      auto array = std::make_shared<emit::Reference>("#in_0");
      assert(mcrl2::data::is_application(eqn.rhs()));
      auto appl = mcrl2::data::application(eqn.rhs());
      assert(mcrl2::data::is_function_symbol(appl.head()));
      assert(std::string(mcrl2::data::function_symbol(appl.head()).name()) == "@func_update");
      auto indexers = io_types.at(op.name())->get_indexers(trans_expr(appl[1], eqn_context, aux_stmts, aux_vars));
      auto value = trans_expr(appl[2], eqn_context, aux_stmts, aux_vars);

      statements.push_back(
        std::make_shared<emit::Assignment>(
          std::make_shared<emit::Index>(array, indexers),
          value
        )
      );
    }
    else
    {
      statements.push_back(
          std::make_shared<emit::Assignment>(
              std::make_shared<emit::Reference>("#" + std::string(op.name())),
              trans_expr(eqn.rhs(), eqn_context, aux_stmts, aux_vars)));
    }

    ret[ret_indices.at(op.name())].statements.insert(
      ret[ret_indices.at(op.name())].statements.begin(),
      cond_aux_stmts.begin(), cond_aux_stmts.end());
    ret[ret_indices.at(op.name())].variables.insert(
      ret[ret_indices.at(op.name())].variables.begin(),
      aux_vars.begin(), aux_vars.end());
    statements.insert(
      statements.begin(),
      aux_stmts.begin(), aux_stmts.end());

    sel.options.emplace_back(std::move(cond), std::move(statements));
  }

  return ret;
}
