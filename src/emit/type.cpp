#include "emit/type.hpp"
#include "translate.hpp"

using namespace emit;
using namespace trans;

std::vector<int> Type::get_indexing_bounded() const {
  auto indexing = get_indexing();
  if (gctx.bounds.contains(name)) {
    auto bounds = gctx.bounds.at(name);
    size_t i = 0;

    for (size_t j = 0; j < indexing.size(); j++) {
      if (indexing[j] == -1)
        indexing[j] = bounds[i++];
      if (i >= bounds.size())
        break;
    }
  }
  return indexing;
}
