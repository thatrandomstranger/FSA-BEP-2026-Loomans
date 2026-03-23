#pragma once
#include <map>
#include <string>
#include <mcrl2/lps/specification.h>
#include "emit/function.hpp"
#include "emit/expression.hpp"
#include "emit/type.hpp"

namespace trans {

struct Context {
  std::map<std::string, std::string> vars = {};
  std::map<std::string, std::string> types = {};
  std::map<std::string, std::string> symbs = {};
};

std::shared_ptr<emit::Expression> trans_expr(
  const mcrl2::data::data_expression&,
  const Context&
);

std::shared_ptr<emit::Expression> trans_appl(
  const mcrl2::data::application&,
  const Context&
);

std::vector<emit::Function> trans_maps(
  const mcrl2::lps::specification&,
  Context&
);

emit::Type trans_sort(
  const mcrl2::data::alias&
);

}
