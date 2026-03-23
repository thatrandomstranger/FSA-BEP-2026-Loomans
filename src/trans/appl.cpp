#include <set>
#include "translate.hpp"
#include "emit/binary.hpp"
#include "emit/reference.hpp"

using namespace trans;

static const std::set<std::string> binary_operators {
  ">", "<", ">=", "<=", "+", "-"
};

std::shared_ptr<emit::Expression> trans::trans_appl(
  const mcrl2::data::application& appl,
  const Context& context
) {
  auto args = std::vector<std::shared_ptr<emit::Expression>>();
  for (const auto& a : appl) {
    args.push_back(trans_expr(a, context));
  }

  if (mcrl2::data::is_function_symbol(appl.head())) {
    auto symb = mcrl2::data::function_symbol(appl.head());
    auto name = std::string(symb.name());
    if (binary_operators.contains(name)) {
      assert(args.size() == 2);
      return std::make_shared<emit::Binary>(name, args);
    } else if (name == "@most_significant_digitNat") {
      assert(args.size() == 1);
      return args[0];
    }
  }

  std::cerr << "APPL: " << appl << " - " << appl.head() << " - " << appl.head().function() << std::endl;
  std::cerr << appl.size() << std::endl;
  return std::make_shared<emit::Reference>("_app");
}