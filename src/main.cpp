#include <string>
#include <iostream>
#include <fstream>

#include <mcrl2/lps/linearise.h>
#include <mcrl2/lps/parse.h>
#include <mcrl2/lps/print.h>
#include "translate.hpp"
#include "emit/enum_type.hpp"
#include "emit/reference_type.hpp"
#include <yaml-cpp/yaml.h>


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
  const auto text = read_file("../../test/belt.mcrl2");

  auto lin = mcrl2::lps::linearise(text);
  auto spec = mcrl2::lps::remove_stochastic_operators(lin);

  auto config = YAML::LoadFile("../../test/config.yaml");
  for (auto b : config["bounds"]) {
    trans::gctx.bounds.insert_or_assign(b.first.as<std::string>(), b.second.as<std::vector<int>>());
  }

  trans::gctx.types.insert_or_assign("Nat",
    std::make_shared<emit::RefType>("INT")
  );
  trans::gctx.types.insert_or_assign("Pos",
    std::make_shared<emit::RefType>("INT")
  );
  trans::gctx.types.insert_or_assign("Bool",
    std::make_shared<emit::RefType>("BOOL")
  );

  auto context = trans::Context {
    .symbs = {
      {"true", "TRUE"},
      {"false", "FALSE"}
    }
  };
  auto sorts = std::vector<std::shared_ptr<emit::Type>> {};
  for (const auto& alias : spec.data().user_defined_aliases()) {
    sorts.push_back(trans::trans_sort(alias));
    trans::gctx.types.insert_or_assign(sorts.back()->name, sorts.back());
    if (auto et = dynamic_cast<emit::EnumType*>(sorts.back().get())) {
      for (const auto op : et->options)
        context.symbs.insert_or_assign(op, 
          std::format("{}.{}", sorts.back()->name, op));
    }
  }
  auto maps = trans::trans_maps(spec, context);

  for (const auto& s : sorts)
    std::cout << *s << '\n';
  for (const auto& m : maps)
    std::cout << m << '\n';

  auto proc = spec.process();
  // std::cout << trans::trans_proc(proc, context) << std::endl;
}
