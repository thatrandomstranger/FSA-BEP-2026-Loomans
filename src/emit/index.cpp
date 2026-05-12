#include "emit/index.hpp"
#include <assert.h>

using namespace emit;

void Index::emit(std::ostream& os) const {
  assert(indexers.size() >= 1);
  os << *array << '[' << *indexers[0];
  for (int i = 1; i < indexers.size(); i++) 
    os << ", " << *indexers[i];
  os << ']';
}
