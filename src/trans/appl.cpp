#include <set>
#include "translate.hpp"
#include "emit/binary.hpp"
#include "emit/reference.hpp"
#include "emit/member_access.hpp"
#include "emit/application.hpp"

using namespace trans;

static const std::set<std::string> binary_operators {
  ">", "<", ">=", "<=", "+", "-", "&&"
};

static const std::set<std::string> ignore {
  "@most_significant_digitNat", "@most_significant_digit", "Pos2Nat"
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
    } else if (context.symbs.contains(name)) {
      return std::make_shared<emit::Application>(
        std::make_shared<emit::Reference>(context.symbs.at(name)),
        args
      );
    } else if (ignore.contains(name)) {
      assert(args.size() == 1);
      return args[0];
    } else if (gctx.struct_comps.contains(name)) {
      return std::make_shared<emit::MemberAccessExpression>(args[0], name);
    } else if (gctx.recognizers.contains(name)) {
      return std::make_shared<emit::Binary>(
        "=", std::vector{args[0], gctx.recognizers.at(name)}
      );
    }
  }

  std::cerr << "APPL: " << appl << " - " << appl.head() << " - " << appl.head().function() << " - ";
  std::cerr << appl.size() << std::endl;
  return std::make_shared<emit::Reference>("_app");
}