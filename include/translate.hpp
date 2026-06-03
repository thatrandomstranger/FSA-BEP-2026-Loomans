#pragma once
#include <map>
#include <set>
#include <string>
#include <mcrl2/lps/specification.h>
#include <mcrl2/lps/linear_process.h>
#include "emit/function.hpp"
#include "emit/function_block.hpp"
#include "emit/expression.hpp"
#include "emit/type.hpp"

namespace trans {

struct Action {
  struct Parameter {
    std::string name;
    bool is_input;
    std::string transform;
  };

  std::string fb;
  std::string cond;
  std::vector<Parameter> params;
};

struct GlobalContext {
  std::map<std::string, std::vector<int>> bounds = {};
  std::map<std::string, std::shared_ptr<emit::Type>> types = {};
  std::map<std::pair<std::string, std::string>, std::shared_ptr<emit::Type>> fa_types = {};
  std::set<std::string> struct_comps = {};
  std::map<std::string, std::shared_ptr<emit::Expression>> constants = {};
  std::map<std::string, std::shared_ptr<emit::Expression>> recognizers = {};
  std::map<std::string, std::string> symbs = {};
  std::map<std::string, emit::Function> func_symbs = {};
  std::vector<emit::Function> aux_functions = {};

  std::map<std::string, Action> actions = {}; 
};

inline GlobalContext gctx;

struct Context {
  std::map<std::string, emit::Variable> vars = {};
};

std::shared_ptr<emit::Expression> trans_expr(
  const mcrl2::data::data_expression&,
  const Context&,
  std::vector<std::shared_ptr<emit::Statement>>& aux_stmts,
  std::vector<emit::Variable>& aux_vars
);

std::shared_ptr<emit::Expression> trans_appl(
  const mcrl2::data::application&,
  const Context&,
  std::vector<std::shared_ptr<emit::Statement>>& aux_stmts,
  std::vector<emit::Variable>& aux_vars
);

std::vector<emit::Function> trans_maps(
  const mcrl2::lps::specification&,
  const Context&
);

std::shared_ptr<emit::Type> trans_sort(
  const mcrl2::data::alias&
);

emit::FunctionBlock trans_proc(
  const mcrl2::lps::linear_process&,
  const mcrl2::lps::process_initializer&,
  Context
);

std::shared_ptr<emit::Expression> get_ternary(
  const mcrl2::data::application& appl,
  const Context& context
);

std::shared_ptr<emit::Expression> trans_binder(
  const mcrl2::data::abstraction&,
  Context,
  std::vector<std::shared_ptr<emit::Statement>>& aux_stmts,
  std::vector<emit::Variable>& aux_vars
);

}
