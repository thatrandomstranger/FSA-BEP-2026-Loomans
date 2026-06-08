#include <set>
#include "translate.hpp"
#include "emit/binary.hpp"
#include "emit/reference.hpp"
#include "emit/member_access.hpp"
#include "emit/application.hpp"
#include "emit/array_type.hpp"
#include "emit/index.hpp"
#include "emit/fb_call.hpp"

using namespace trans;

static const std::map<std::string, std::string> binary_operators{
    {">", ">"}, {"<", "<"}, {">=", ">="}, {"<=", "<="}, {"+", "+"}, {"-", "-"}, {"&&", "AND"}, {"||", "OR"}, {"==", "="}, {"!=", "!="}};

static const std::set<std::string> ignore{
    "@most_significant_digitNat", "@most_significant_digit", "Pos2Nat"};

std::shared_ptr<emit::Expression> trans::trans_appl(
    const mcrl2::data::application &appl,
    const Context &context,
    std::vector<std::shared_ptr<emit::Statement>> &aux_stmts,
    std::vector<emit::Variable> &aux_vars)
{
  auto args = std::vector<std::shared_ptr<emit::Expression>>();
  for (const auto &a : appl)
  {
    args.push_back(trans_expr(a, context, aux_stmts, aux_vars));
  }

  if (mcrl2::data::is_function_symbol(appl.head()))
  {
    auto symb = mcrl2::data::function_symbol(appl.head());
    auto name = std::string(symb.name());

    if (gctx.func_symbs.contains(name))
    {
      auto func = gctx.func_symbs.at(name);
      auto ret = std::make_shared<emit::FBCall>(func.name);
      auto io = func.io;
      for (int i = 0; auto a : args)
      {
        if (io)
        {
          ret->parameters.emplace_back("in_0", a, true);
          io = false;
          continue;
        }
        ret->parameters.emplace_back(func.inputs[i++].name, a, true);
      }
      return ret;
    }
    else if (gctx.symbs.contains(name))
    {
      return std::make_shared<emit::Application>(
          std::make_shared<emit::Reference>(gctx.symbs.at(name)),
          args);
    }
    else if (ignore.contains(name))
    {
      assert(args.size() == 1);
      return args[0];
    }
    else if (gctx.struct_comps.contains(name))
    {
      return std::make_shared<emit::MemberAccessExpression>(args[0], name);
    }
    else if (name == "==" && mcrl2::data::is_application(appl[1]) &&
             mcrl2::data::is_function_symbol(mcrl2::data::application(appl[1]).head()) && gctx.constructors.contains(mcrl2::data::function_symbol(mcrl2::data::application(appl[1]).head()).name()))
    {
      auto appl2 = mcrl2::data::application(appl[1]);
      auto stype = gctx.constructors.at(mcrl2::data::function_symbol(appl2.head()).name());
      auto ret = std::make_shared<emit::Binary>("AND");
      for (int i = 0; const auto &a : appl2)
      {
        auto val = trans_expr(a, context, aux_stmts, aux_vars);
        auto check = std::make_shared<emit::Binary>("=");
        check->values.push_back(std::make_shared<emit::MemberAccessExpression>(args[0], stype->components[i++].first));
        check->values.push_back(val);
        ret->values.push_back(check);
      }
      return ret;
    }
    else if (binary_operators.contains(name))
    {
      assert(args.size() == 2);
      return std::make_shared<emit::Binary>(binary_operators.at(name), args);
    }
    else if (gctx.recognizers.contains(name))
    {
      return std::make_shared<emit::Binary>(
          "=", std::vector{args[0], gctx.recognizers.at(name)});
    }
    else if (name == "=>")
    {
      auto not_arg0 = std::make_shared<emit::Application>(
          std::make_shared<emit::Reference>("NOT"),
          std::vector<std::shared_ptr<emit::Expression>>{args[0]});
      return std::make_shared<emit::Binary>(
          "OR", std::vector<std::shared_ptr<emit::Expression>>{not_arg0, args[1]});
    }
    else if (name == "@natpred")
    {
      return std::make_shared<emit::Binary>(
          "-", std::vector<std::shared_ptr<emit::Expression>>{args[0],
                                                              std::make_shared<emit::Reference>("1")});
    }
    else if (name == "@not_equals_zero")
    {
      return std::make_shared<emit::Binary>(
          "!=", std::vector<std::shared_ptr<emit::Expression>>{args[0],
                                                               std::make_shared<emit::Reference>("0")});
    }
    else if (name == "if")
    {
      return std::make_shared<emit::FBCall>(
          get_ternary(appl, context)->ref,
          std::vector<emit::FBCall::Argument>{
              {"c", args[0], true},
              {"e1", args[1], true},
              {"e0", args[2], true}});
    }
  }

  if (mcrl2::data::is_variable(appl.head()))
  {
    auto var = mcrl2::data::variable(appl.head());
    assert(mcrl2::data::is_basic_sort(appl.begin()->sort()));
    auto dom_sort = mcrl2::data::basic_sort(appl.begin()->sort());
    if (context.vars.contains(var.name()))
      return std::make_shared<emit::Index>(
          std::make_shared<emit::Reference>("#" + context.vars.at(var.name()).name),
          gctx.types.at(dom_sort.name())->get_indexers(args[0]));
  }

  std::cerr << "APPL: " << appl << " - " << appl.head() << " - " << appl.head().function() << " - ";
  std::cerr << appl.size() << std::endl;
  return std::make_shared<emit::Reference>("_app");
}