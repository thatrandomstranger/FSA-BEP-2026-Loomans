#pragma once
#include "emit/variable.hpp"
#include "emit/statement.hpp"
#include <vector>
#include <memory>

namespace emit {

struct FunctionBlock {
  std::string name;
  std::vector<Variable> inputs = {};
  std::vector<Variable> outputs = {};
  std::vector<Variable> variables = {};
  std::vector<std::shared_ptr<Statement>> statements = {};


  void emit(std::ostream&) const;
}; 

}

std::ostream& operator<<(std::ostream&, const emit::FunctionBlock&);
