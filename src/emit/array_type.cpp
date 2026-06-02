#include "emit/array_type.hpp"
#include "emit/indentation.hpp"
#include "translate.hpp"

using namespace emit;

ArrayType::ArrayType(std::shared_ptr<Type> domain,
                    std::shared_ptr<Type> range) 
  : Type(""),
    domain(domain),
    range(range) {
  std::stringstream n;
  n << "ARRAY [";
  for (bool first = true; auto dim : domain->get_indexing_bounded())
  {
    if (!first)
      n << ", ";
    first = false;
    n << "0.." << dim-1;
  }
  n << "] OF ";
  n << range->name;
  name = n.str();
}

void ArrayType::emit(std::ostream &os) const
{
  // os << indent << "TYPE " << name << " :\n";
  // os << indent++ << "ARRAY [\n";

  // for (bool first = true; auto dim : domain->get_indexing_bounded())
  // {
  //   if (first)
  //     os << indent;
  //   else
  //     os << ", ";
  //   first = false;
  //   os << "0.." << dim;
  // }
  // os << '\n' << --indent << "] OF " << range->name << ";\n";
  // os << indent << "END_TYPE\n";
}
