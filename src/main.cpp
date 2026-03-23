#include <string>
#include <iostream>
#include <fstream>

#include "mcrl2/lps/linearise.h"
#include "mcrl2/lps/parse.h"
#include "mcrl2/lps/print.h"
#include "translate.hpp"
#include "emit/type.hpp"


// https://stackoverflow.com/questions/116038/how-do-i-read-an-entire-file-into-a-stdstring-in-c
auto read_file(std::string_view path) -> std::string {
    constexpr auto read_size = std::size_t(4096);
    auto stream = std::ifstream(path.data());
    stream.exceptions(std::ios_base::badbit);

    if (not stream) {
        throw std::ios_base::failure("file does not exist");
    }
    
    auto out = std::string();
    auto buf = std::string(read_size, '\0');
    while (stream.read(& buf[0], read_size)) {
        out.append(buf, 0, stream.gcount());
    }
    out.append(buf, 0, stream.gcount());
    return out;
}

int main() {
  const auto text = read_file("../../test/test.mcrl2");

  auto lin = mcrl2::lps::linearise(text);
  auto spec = mcrl2::lps::remove_stochastic_operators(lin);

  auto context = trans::Context {
    .types = {
      {"Nat", "INT"},
      {"Pos", "INT"},
      {"Bool", "BOOL"}
    },
    .symbs = {
      {"true", "TRUE"},
      {"false", "FALSE"}
    }
  };
  auto sorts = std::vector<emit::Type> {};
  for (const auto& alias : spec.data().user_defined_aliases()) {
    sorts.push_back(trans::trans_sort(alias));
    context.types.insert_or_assign(sorts.back().name, sorts.back().name);
    for (const auto op : sorts.back().options)
      context.symbs.insert_or_assign(op, 
        std::format("{}.{}", sorts.back().name, op));
  }
  auto maps = trans::trans_maps(spec, context);

  for (const auto& s : sorts)
    std::cout << s << '\n';
  for (const auto& m : maps)
    std::cout << m << '\n';
}
