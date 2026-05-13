#pragma once
#include "emit/variable.hpp"
#include "emit/statement.hpp"
#include <vector>
#include <memory>

namespace emit {

struct Function {
  std::string name;
  std::shared_ptr<Type> type;
  std::vector<Variable> inputs;
  std::vector<std::shared_ptr<Statement>> statements = {};
  bool io = false;


  void emit(std::ostream&) const;
}; 

}

std::ostream& operator<<(std::ostream&, const emit::Function&);
