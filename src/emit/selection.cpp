#include "emit/selection.hpp"
#include "emit/indentation.hpp"

using namespace emit;

void Selection::emit(std::ostream& os) const {
  bool first = true;
  for (const auto& opt : options) {
    os << indent++ << (first ? "IF " : "ELSIF ") << *opt.condition << " THEN\n";
    for (const auto& st : opt.statements) {
      if (auto expr = dynamic_cast<emit::Expression*>(st.get()))
        os << indent << *expr << ";\n";
      else
        os << *st;
    }
    os << indent_down;
    first = false;
  }
  if (else_stmts.size() > 0) {
    os << indent++ << "ELSE\n";
     for (const auto& st : else_stmts) {
      if (auto expr = dynamic_cast<emit::Expression*>(st.get()))
        os << indent << *expr << ";\n";
      else
        os << *st;
    }
    os << indent_down;
  }
  os << indent << "END_IF;\n";
}
