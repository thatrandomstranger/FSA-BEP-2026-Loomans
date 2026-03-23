#pragma once
#include <ostream>

namespace emit {
  namespace detail {
    struct indent_post_up {};
    struct indent_post_down {};
    struct indent_pre_up {};
    struct indent_pre_down {};
    struct indent {
      detail::indent_pre_up operator++() { return {}; }
      detail::indent_pre_down operator--() { return {}; }
      detail::indent_post_up operator++(int) { return {}; }
      detail::indent_post_down operator--(int) { return {}; }
    };
    struct indent_up {};
    struct indent_down {};
    struct indent_reset {};
  }

  inline detail::indent indent;
  inline detail::indent_up indent_up;
  inline detail::indent_down indent_down;
  inline detail::indent_reset indent_reset;
}

std::ostream& operator<<(std::ostream&, emit::detail::indent);
std::ostream& operator<<(std::ostream&, emit::detail::indent_up);
std::ostream& operator<<(std::ostream&, emit::detail::indent_down);
std::ostream& operator<<(std::ostream&, emit::detail::indent_reset);
inline std::ostream& operator<<(std::ostream& os, 
                                emit::detail::indent_pre_up) {
  return os << emit::indent_up << emit::indent;
}
inline std::ostream& operator<<(std::ostream& os, 
                                emit::detail::indent_pre_down) {
  return os << emit::indent_down << emit::indent;
}
inline std::ostream& operator<<(std::ostream& os, 
                                emit::detail::indent_post_up) {
  return os << emit::indent << emit::indent_up;
}
inline std::ostream& operator<<(std::ostream& os, 
                                emit::detail::indent_post_down) {
  return os << emit::indent << emit::indent_down;
}
