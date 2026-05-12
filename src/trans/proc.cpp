#include <map>
#include <set>
#include <format>
#include "translate.hpp"

using namespace trans;

emit::Function trans::trans_proc(
  const mcrl2::lps::linear_process& proc,
  const Context& context
) {
  auto ret = emit::Function {
    .name = "mainloop",
  };

  std::cout << proc << std::endl;

  for (auto sum : proc.action_summands()) {
    std::cout << sum << std::endl;

    std::cout << "ASSIGNMENTS:" << std::endl;
    for (const auto& ass : sum.assignments())
      std::cout << ass << std::endl;

    std::cout << "SUM VARS:" << std::endl;
    for (const auto& ass : sum.summation_variables())
      std::cout << ass.name() << " - " << ass.sort() << std::endl;

    std::cout << "CONDITION:" << std::endl;
    std::cout << sum.condition() << std::endl;

    std::cout << "ACTION:" << std::endl;
    std::cout << sum.multi_action() << std::endl;
  }
  return ret;
}