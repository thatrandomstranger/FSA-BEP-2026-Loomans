#include <format>
#include "translate.hpp"
#include "emit/reference.hpp"
#include "emit/iterate.hpp"
#include "emit/assignment.hpp"
#include "emit/struct_type.hpp"
#include "emit/binary.hpp"

using namespace trans;

std::shared_ptr<emit::Expression> trans::trans_binder(
    const mcrl2::data::abstraction &expr,
    Context context,
    std::vector<std::shared_ptr<emit::Statement>> &aux_stmts,
    std::vector<emit::Variable> &aux_vars)
{
  assert(expr.variables().size() == 1);
  auto var = *expr.variables().begin();
  assert(mcrl2::data::is_basic_sort(var.sort()));
  auto sort = mcrl2::data::basic_sort(var.sort());
  auto type = gctx.types.at(sort.name());
  auto indexing = type->get_indexing_bounded();
  auto iterate = std::make_shared<emit::Iterate>();
  if (auto stype = dynamic_cast<emit::StructType *>(type.get()))
  {
    aux_vars.push_back({"binder_var", type});
    context.vars.insert_or_assign(var.name(), aux_vars.back());
    for (int i = 0; auto dim : indexing)
    {
      aux_vars.push_back({std::format("iter_{}", i), gctx.types.at("Nat")});
      auto comp = std::make_shared<emit::Reference>(
          std::format("#binder_var.{}", stype->components[i].first));
      iterate->dims.push_back({dim, std::format("iter_{}", i), comp});
      i++;
    }
  }
  else
  {
    throw std::runtime_error(std::format("Unsupported forall expression on sort",
                                         std::string(sort.name())));
  }

  aux_vars.push_back({"binder_res", gctx.types.at("Bool")});
  aux_stmts.push_back(std::make_shared<emit::Assignment>(
      std::make_shared<emit::Reference>("#binder_res"),
      std::make_shared<emit::Reference>(mcrl2::data::is_forall(expr) ? "TRUE" : "FALSE")));

  std::vector<std::shared_ptr<emit::Statement>> cond_aux_stmts;
  auto cond = trans_expr(expr.body(), context, cond_aux_stmts, aux_vars);
  iterate->stmts.insert(iterate->stmts.begin(),
                        cond_aux_stmts.begin(), cond_aux_stmts.end());
  iterate->stmts.push_back(std::make_shared<emit::Assignment>(
      std::make_shared<emit::Reference>("#binder_res"),
      std::make_shared<emit::Binary>(
          mcrl2::data::is_forall(expr) ? "AND" : "OR",
          std::vector<std::shared_ptr<emit::Expression>>{
              std::make_shared<emit::Reference>("#binder_res"),
              cond})));

  aux_stmts.push_back(iterate);
  return std::make_shared<emit::Reference>("#binder_res");
}
