#include <format>
#include "translate.hpp"
#include "emit/reference.hpp"

std::shared_ptr<emit::Expression> trans::trans_expr(
  const mcrl2::data::data_expression& expr,
  const trans::Context& context
) {
  if (mcrl2::data::is_function_symbol(expr)) {
    auto symb = mcrl2::data::function_symbol(expr);
    if (context.symbs.contains(symb.name()))
      return std::make_shared<emit::Reference>(context.symbs.at(symb.name()));
  } else if (mcrl2::data::is_application(expr)) {
    return trans_appl(mcrl2::data::application(expr), context);
  } else if (expr.type_is_int()) {
    auto val = atermpp::aterm_int(expr);
    return std::make_shared<emit::Reference>(std::format("{}", val.value()));
  } else if (mcrl2::data::is_variable(expr)) {
    auto var = mcrl2::data::variable(expr);
    if (context.vars.contains(var.name())) 
      return std::make_shared<emit::Reference>(context.vars.at(var.name()));
  }

  std::cerr << "UNKN: " << expr << " - " <<  expr.function() << std::endl;
  return std::make_shared<emit::Reference>("_unk");
}
