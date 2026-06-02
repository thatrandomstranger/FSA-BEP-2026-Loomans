#include <format>
#include "translate.hpp"
#include "emit/reference.hpp"

std::shared_ptr<emit::Expression> trans::trans_expr(
  const mcrl2::data::data_expression& expr,
  const trans::Context& context,
  std::vector<std::shared_ptr<emit::Statement>>& aux_stmts,
  std::vector<emit::Variable>& aux_vars
) {
  if (mcrl2::data::is_function_symbol(expr)) {
    auto symb = mcrl2::data::function_symbol(expr);
    if (gctx.symbs.contains(symb.name()))
      return std::make_shared<emit::Reference>(gctx.symbs.at(symb.name()));
    if (gctx.constants.contains(symb.name()))
      return gctx.constants.at(symb.name());
    std::cerr << "UNKS: " << expr << " - " <<  symb.name() << std::endl;
    return std::make_shared<emit::Reference>("_unks");
  } else if (mcrl2::data::is_application(expr)) {
    return trans_appl(mcrl2::data::application(expr), context, aux_stmts, aux_vars);
  } else if (expr.type_is_int()) {
    auto val = atermpp::aterm_int(expr);
    return std::make_shared<emit::Reference>(std::format("{}", val.value()));
  } else if (mcrl2::data::is_variable(expr)) {
    auto var = mcrl2::data::variable(expr);
    if (context.vars.contains(var.name())) 
      return std::make_shared<emit::Reference>("#" + context.vars.at(var.name()).name);
  } else if (mcrl2::data::is_forall(expr) || mcrl2::data::is_exists(expr)) {
    return trans_binder(mcrl2::data::abstraction(expr), context, aux_stmts, aux_vars);
  }

  std::cerr << "UNKN: " << expr << " - " <<  expr.function() << std::endl;
  return std::make_shared<emit::Reference>("_unk");
}
