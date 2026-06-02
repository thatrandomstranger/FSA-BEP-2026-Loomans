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
      for (int i = 0; auto dim : indexing) {
        auto iter = std::format("iter_{}", i++);
        aux_vars.push_back({iter, gctx.types.at("Nat")});
        iterate->dims.push_back({dim, iter});
        index->indexers.push_back(std::make_shared<emit::Reference>(iter));
      }
      iterate->stmts.push_back(
        std::make_shared<emit::Assignment>(
          index, trans_expr(rhs, context, stmts, aux_vars)
        )
      );
      stmts.push_back(iterate);
    }
  }
  else if (auto stype = dynamic_cast<emit::StructType *>(var.type.get()); stype 
    && mcrl2::data::is_application(rhs))
  {
    auto con = mcrl2::data::application(rhs);

    for (size_t i = 0; auto p : con) {
      stmts.push_back(
        std::make_shared<emit::Assignment>(
          std::make_shared<emit::Reference>("#" + var.name + "." + stype->components[i++].first),
          trans_expr(p, context, stmts, aux_vars))
      );
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
    const mcrl2::lps::process_initializer& init,
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
      .variables = {emit::Variable("execute", gctx.types.at("Bool")), emit::Variable("done", gctx.types.at("Bool"))},
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
  std::vector<emit::Variable> aux_vars;
  for (auto i = variables_start_parameters; auto arg : init.expressions()) {
    for (auto st : trans_assignment(ret.variables[i++], arg, context, aux_vars))
      {
        selection->options.front().statements.push_back(std::move(st));
      }
  }

  for (auto sum : proc.action_summands())
  {
    for (auto var : sum.summation_variables())
    {
      ret.variables.push_back(emit::Variable{
          var.name(),
          gctx.types.at(mcrl2::data::basic_sort(var.sort()).name())});
      context.vars.insert_or_assign(var.name(), ret.variables.back());
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
    auto fb_call = std::make_shared<emit::FBCall>(action.fb);
    for (int i = 0; auto a : action_args)
    {
      if (i >= action.params.size())
        break;
      fb_call->parameters.push_back(
          {.name = action.params[i].name,
           .arg = trans_expr(a, context, aux_stmts, aux_vars),
           .is_input = action.params[i].is_input});
      i++;
    }
    fb_call->parameters.push_back({.name = "execute",
                                   .arg = std::make_shared<emit::Reference>("#execute"),
                                   .is_input = true});
    fb_call->parameters.push_back({.name = "done",
                                   .arg = std::make_shared<emit::Reference>("#done"),
                                   .is_input = false});

    option.statements.push_back(std::move(fb_call));

    auto on_done = std::make_shared<emit::Selection>();
    on_done->options.push_back({std::make_shared<emit::Reference>("#done")});
    on_done->options.back().statements.push_back(
        std::make_shared<emit::Assignment>(
            std::make_shared<emit::Reference>("#execute"),
            std::make_shared<emit::Reference>("TRUE")));
    for (auto da : sum.assignments())
    {
      assert(mcrl2::data::is_variable(da.lhs()));
      auto var = context.vars.at(mcrl2::data::variable(da.lhs()).name());

      for (auto st : trans_assignment(var, da.rhs(), context, aux_vars))
      {
        on_done->options.back().statements.push_back(std::move(st));
      }
    }
    on_done->options.back().statements.push_back(
        std::make_shared<emit::Reference>("RETURN"));

    option.statements.push_back(std::move(on_done));
    selection->options.push_back(std::move(option));
  }

  std::set<std::string> aux_vars_seen;
  for (const auto& var : aux_vars) {
    if (aux_vars_seen.contains(var.name))
      continue;
    ret.variables.push_back(var);
    aux_vars_seen.insert(var.name);
  }

  return ret;
}