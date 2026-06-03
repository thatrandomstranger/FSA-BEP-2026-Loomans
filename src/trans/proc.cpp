#include <map>
#include <set>
#include <format>
#include "translate.hpp"
#include "emit/selection.hpp"
#include "emit/application.hpp"
#include "emit/fb_call.hpp"
#include "emit/reference.hpp"
#include "emit/assignment.hpp"
#include "emit/array_type.hpp"
#include "emit/struct_type.hpp"
#include "emit/enum_type.hpp"
#include "emit/iterate.hpp"
#include "emit/index.hpp"

using namespace trans;

static std::vector<std::shared_ptr<emit::Statement>> trans_assignment(
    const emit::Variable &var,
    mcrl2::data::data_expression rhs,
    const Context &context,
    std::vector<emit::Variable> &aux_vars)
{
  std::vector<std::shared_ptr<emit::Statement>> stmts;
  if (auto arr_type = dynamic_cast<emit::ArrayType *>(var.type.get()))
  {
    if (mcrl2::data::is_application(rhs))
    {
      stmts.push_back(trans_expr(rhs, context, stmts, aux_vars));
    }
    else if (mcrl2::data::is_variable(rhs))
    {
    }
    else
    {

      auto iterate = std::make_shared<emit::Iterate>();
      auto indexing = arr_type->domain->get_indexing_bounded();
      auto index = std::make_shared<emit::Index>(
          std::make_shared<emit::Reference>("#" + var.name));
      for (int i = 0; auto dim : indexing)
      {
        auto iter = std::format("iter_{}", i++);
        aux_vars.push_back({iter, gctx.types.at("Nat")});
        iterate->dims.push_back({dim, iter});
        index->indexers.push_back(std::make_shared<emit::Reference>("#" + iter));
      }
      iterate->stmts.push_back(
          std::make_shared<emit::Assignment>(
              index, trans_expr(rhs, context, stmts, aux_vars)));
      stmts.push_back(iterate);
    }
  }
  else if (auto stype = dynamic_cast<emit::StructType *>(var.type.get()); stype && mcrl2::data::is_application(rhs))
  {
    auto con = mcrl2::data::application(rhs);

    for (size_t i = 0; auto p : con)
    {
      stmts.push_back(
          std::make_shared<emit::Assignment>(
              std::make_shared<emit::Reference>("#" + var.name + "." + stype->components[i++].first),
              trans_expr(p, context, stmts, aux_vars)));
    }
    return stmts;
  }
  else
  {
    return {std::make_shared<emit::Assignment>(
        std::make_shared<emit::Reference>("#" + var.name),
        trans_expr(rhs, context, stmts, aux_vars))};
  }
  return stmts;
}

emit::FunctionBlock trans::trans_proc(
    const mcrl2::lps::linear_process &proc,
    const mcrl2::lps::process_initializer &init,
    Context context)
{
  auto ret = emit::FunctionBlock{
      .name = "\"FB_Generated_Controller\"",
      .inputs = {
          emit::Variable("initialize", gctx.types.at("Bool")),
      },
      .outputs = {
          emit::Variable("error", gctx.types.at("Bool")),
      },
      .variables = {emit::Variable("execute", gctx.types.at("Bool"))},
  };

  size_t variables_start_parameters = ret.variables.size();
  for (auto param : proc.process_parameters())
  {
    if (mcrl2::data::is_basic_sort(param.sort()))
    {
      ret.variables.push_back(emit::Variable{
          param.name(),
          gctx.types.at(mcrl2::data::basic_sort(param.sort()).name())});
      context.vars.insert_or_assign(param.name(), ret.variables.back());
    }
    else if (mcrl2::data::is_function_sort(param.sort()))
    {
      auto func = mcrl2::data::function_sort(param.sort());
      assert(mcrl2::data::is_basic_sort(func.domain().front()));
      assert(mcrl2::data::is_basic_sort(func.codomain()));
      ret.variables.push_back(emit::Variable{
          param.name(),
          gctx.fa_types.at({mcrl2::data::basic_sort(func.domain().front()).name(),
                            mcrl2::data::basic_sort(func.codomain()).name()})});
      context.vars.insert_or_assign(param.name(), ret.variables.back());
    }
    else
    {
      std::cerr << "UNKS " << param << " - " << param.sort() << " - " << param.sort().function() << std::endl;
    }
  }

  // std::cout << proc << std::endl;
  auto selection = std::make_shared<emit::Selection>();
  ret.statements.push_back(selection);

  selection->options.push_back({std::make_shared<emit::Reference>("#initialize"),
                                {}});
  selection->options.back().statements.push_back(
      std::make_shared<emit::Assignment>(
          std::make_shared<emit::Reference>("#execute"),
          std::make_shared<emit::Reference>("TRUE")));
  selection->options.back().statements.push_back(
      std::make_shared<emit::Assignment>(
          std::make_shared<emit::Reference>("#initialize"),
          std::make_shared<emit::Reference>("FALSE")));
  std::vector<emit::Variable> aux_vars;
  for (auto i = variables_start_parameters; auto arg : init.expressions())
  {
    for (auto st : trans_assignment(ret.variables[i++], arg, context, aux_vars))
    {
      selection->options.front().statements.push_back(std::move(st));
    }
  }
  selection->options.front().statements.push_back(
      std::make_shared<emit::Reference>("RETURN"));

  for (auto sum : proc.action_summands())
  {
    auto selection = std::make_shared<emit::Selection>();

    std::set<std::string> svars;
    for (auto var : sum.summation_variables())
    {
      ret.variables.push_back(emit::Variable{
          var.name(),
          gctx.types.at(mcrl2::data::basic_sort(var.sort()).name())});
      context.vars.insert_or_assign(var.name(), ret.variables.back());
      svars.insert(var.name());
    }

    std::vector<std::shared_ptr<emit::Statement>> aux_stmts;
    auto option = emit::Selection::Option(trans_expr(sum.condition(), context, aux_stmts, aux_vars));
    auto action_name = sum.multi_action().actions().front().label().name();
    auto action_args = sum.multi_action().actions().front().arguments();
    if (!gctx.actions.contains(action_name))
    {
      std::cerr << "Unknown action: " << action_name << std::endl;
      continue;
    }
    auto action = gctx.actions.at(action_name);
    if (action.fb.size() > 0)
    {
      auto on_execute = std::make_shared<emit::Selection>();
      on_execute->options.push_back({std::make_shared<emit::Reference>("#execute")});
      auto &stmts_execute = on_execute->options.back().statements;
      stmts_execute.push_back(std::make_shared<emit::Assignment>(
          std::make_shared<emit::Reference>("#execute"),
          std::make_shared<emit::Reference>("FALSE")));
      stmts_execute.push_back(std::make_shared<emit::Assignment>(
          std::make_shared<emit::Reference>(action.fb + ".Execute"),
          std::make_shared<emit::Reference>("TRUE")));
      for (int i = 0; i < action.params.size(); i++)
      {
        if (!action.params[i].is_input)
          continue;
        auto pvar = std::make_shared<emit::Reference>(
            (action.fb.size() > 0 ? action.fb + "." : "") + action.params[i].name);
        auto te = trans_expr(mcrl2::data::data_expression(action_args[action.params[i].id]), context, aux_stmts, aux_vars);
        if (action.params[i].transform.size() > 0)
        {
          std::stringstream s;
          s << *te;
          auto ts = action.params[i].transform;
          ts.replace(ts.find('$'), 1, s.str());
          te = std::make_shared<emit::Reference>(ts);
        }
        stmts_execute.push_back(std::make_shared<emit::Assignment>(pvar, te));
      }
      option.statements.push_back(on_execute);
    }

    auto on_done = std::make_shared<emit::Selection>();
    if (action.fb.size() > 0)
      on_done->options.push_back({std::make_shared<emit::Reference>(action.fb + ".Done")});
    else
      on_done->options.push_back({std::make_shared<emit::Reference>(action.cond)});
    auto &stmts_done = on_done->options.back().statements;
    stmts_done.push_back(
        std::make_shared<emit::Assignment>(
            std::make_shared<emit::Reference>("#execute"),
            std::make_shared<emit::Reference>("TRUE")));
    
    if (action.fb.size() > 0)
      stmts_done.push_back(
          std::make_shared<emit::Assignment>(
              std::make_shared<emit::Reference>(action.fb + ".Execute"),
              std::make_shared<emit::Reference>("FALSE")));

    for (int i = 0; i < action.params.size(); i++)
    {
      if (action.params[i].is_input)
        continue;
      auto pvar = std::make_shared<emit::Reference>(
          (action.fb.size() > 0 ? action.fb + "." : "") + action.params[i].name);
      assert(mcrl2::data::is_variable(action_args[action.params[i].id]));
      auto tvar = mcrl2::data::variable(action_args[action.params[i].id]);
      svars.extract(tvar.name());
      auto te = trans_expr(mcrl2::data::data_expression(action_args[action.params[i].id]), context, aux_stmts, aux_vars);
      if (action.params[i].transform.size() > 0)
      {
        std::stringstream s;
        s << *te;
        auto ts = action.params[i].transform;
        ts.replace(ts.find('$'), 1, s.str());
        te = std::make_shared<emit::Reference>(ts);
      }
      stmts_done.push_back(std::make_shared<emit::Assignment>(te, pvar));
    }
    for (auto da : sum.assignments())
    {
      assert(mcrl2::data::is_variable(da.lhs()));
      auto var = context.vars.at(mcrl2::data::variable(da.lhs()).name());

      for (auto st : trans_assignment(var, da.rhs(), context, aux_vars))
      {
        stmts_done.push_back(std::move(st));
      }
    }

    option.statements.push_back(std::move(on_done));
    option.statements.push_back(
        std::make_shared<emit::Reference>("RETURN"));
    selection->options.push_back(std::move(option));
    std::shared_ptr<emit::Statement> sumst = selection;
    for (auto var : sum.summation_variables())
    {
      if (!svars.contains(var.name()))
        continue;
      auto tvar = context.vars.at(var.name());
      auto iter = std::make_shared<emit::Iterate>();
      if (auto etype = dynamic_cast<emit::EnumType *>(tvar.type.get()))
      {
        iter->dims.emplace_back(etype->options.size(), tvar.name);
      }
      else if (auto stype = dynamic_cast<emit::StructType *>(tvar.type.get()))
      {
        auto indexing = stype->get_indexing_bounded();
        for (int i = 0; auto dim : indexing)
        {
          aux_vars.push_back({std::format("iter_{}", i), gctx.types.at("Nat")});
          auto comp = std::make_shared<emit::Reference>(
              std::format("#{}.{}", tvar.name, stype->components[i].first));
          iter->dims.push_back({dim, std::format("iter_{}", i), comp});
          i++;
        }
      }
      else if (tvar.type->name == "BOOL")
      {
        aux_vars.push_back({"iter_0", gctx.types.at("Nat")});
        iter->dims.emplace_back(2, "iter_0");
        iter->stmts.push_back(std::make_shared<emit::Assignment>(
            std::make_shared<emit::Reference>("#" + tvar.name),
            std::make_shared<emit::Reference>("#iter_0 = 1")));
      }
      else
      {
        throw std::runtime_error(std::format("Unsupported summation on type {}", tvar.name));
      }
      iter->stmts.push_back(sumst);
      sumst = iter;
    }
    ret.statements.push_back(sumst);
  }

  std::set<std::string> aux_vars_seen;
  for (const auto &var : aux_vars)
  {
    if (aux_vars_seen.contains(var.name))
      continue;
    ret.variables.push_back(var);
    aux_vars_seen.insert(var.name);
  }

  return ret;
}