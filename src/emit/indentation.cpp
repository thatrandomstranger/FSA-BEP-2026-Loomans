#include "emit/indentation.hpp"

static const int xindex = std::ios_base::xalloc();

std::ostream& operator<<(std::ostream& os, emit::detail::indent) {
  if (os.pword(xindex)) {
    const int& indentation = *static_cast<int*>(os.pword(xindex));
    for (int i = 0; i < indentation; i++)
      os << "    ";
  }
  return os;
}

std::ostream& operator<<(std::ostream& os, emit::detail::indent_up) {
  if (!os.pword(xindex)) 
    os.pword(xindex) = new int(0);
  int& indentation = *static_cast<int*>(os.pword(xindex));
  indentation++;
  return os;
}

std::ostream& operator<<(std::ostream& os, emit::detail::indent_down) {
  if (os.pword(xindex)) {
    int& indentation = *static_cast<int*>(os.pword(xindex));
    if (indentation > 0) 
      indentation--;
    else 
      os << emit::indent_reset;
  }
  return os;
}

std::ostream& operator<<(std::ostream& os, emit::detail::indent_reset) {
  if (os.pword(xindex)) {
    delete static_cast<int*>(os.pword(xindex));
    os.pword(xindex) = nullptr;
  }
  return os;
}
