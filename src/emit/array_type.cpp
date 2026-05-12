#include "emit/array_type.hpp"
#include "emit/indentation.hpp"
#include "translate.hpp"

using namespace emit;

void ArrayType::emit(std::ostream &os) const
{
  os << indent << "TYPE " << name << " :\n";
  os << indent++ << "ARRAY [\n";

  for (bool first = true; auto dim : domain->get_indexing_bounded())
  {
    if (first)
      os << indent;
    else
      os << ", ";
    first = false;
    os << "0.." << dim;
  }
  os << '\n' << --indent << "] OF " << domain->name << ";\n";
  os << indent << "END_TYPE\n";
}
