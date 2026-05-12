#pragma once
#include <ostream>
#include <string>
#include <vector>
#include <memory>
#include "emit/expression.hpp"

namespace emit {

struct Type {
  std::string name;

  Type(std::string name) : name(std::move(name)) {}

  virtual void emit(std::ostream&) const = 0;

  virtual std::vector<int> get_indexing() const {
    throw std::runtime_error(
      std::string("Indexing not supported on type: " + name));
  };

  std::vector<int> get_indexing_bounded() const;

  virtual std::vector<std::shared_ptr<Expression>> get_indexers(
    std::shared_ptr<Expression>) const {
    throw std::runtime_error(
      std::string("Indexing not supported on type: " + name));
  };

  virtual ~Type() = default;
};

}

inline std::ostream& operator<<(std::ostream& os, const emit::Type& tp) {
  tp.emit(os);
  return os;
}
