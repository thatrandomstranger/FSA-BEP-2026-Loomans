#pragma once
#include <map>
#include <set>
#include <string>
#include <mcrl2/lps/specification.h>
#include <mcrl2/lps/linear_process.h>
#include "emit/function.hpp"
#include "emit/expression.hpp"
#include "emit/type.hpp"

namespace trans {

struct GlobalContext {
  std::map<std::string, std::vector<int>> bounds = {};
  std::map<std::string, std::shared_ptr<emit::Type>> types = {};
  std::set<std::string> struct_comps = {};
  std::map<std::string, std::shared_ptr<emit::Expression>> constants = {};
  std::map<std::string, std::shared_ptr<emit::Expression>> recognizers = {};
};

inline GlobalContext gctx;

struct Context {
  std::map<std::string, std::string> vars = {};
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

std::shared_ptr<emit::Type> trans_sort(
  const mcrl2::data::alias&
);

emit::Function trans_proc(
  const mcrl2::lps::linear_process&,
  const Context&
);

}
