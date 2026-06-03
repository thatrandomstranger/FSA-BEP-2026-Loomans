#include <string>
#include <iostream>
#include <fstream>

#include <mcrl2/lps/linearise.h>
#include <mcrl2/lps/parse.h>
#include <mcrl2/lps/print.h>
#include <mcrl2/data/rewriters/simplify_rewriter.h>
#include <mcrl2/lps/constelm.h>
#include "translate.hpp"
#include "emit/enum_type.hpp"
#include "emit/struct_type.hpp"
#include "emit/reference_type.hpp"
#include <yaml-cpp/yaml.h>

// #include "mcrl2/data/used_data_equation_selector.h"
#include "mcrl2/lps/stochastic_specification.h"
#include "mcrl2/lps/find.h" // For finding function symbols in the LPS


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

void prune_unused_equations(mcrl2::lps::specification& spec)
{
  std::set<mcrl2::data::function_symbol> used_symbols;
  mcrl2::lps::find_function_symbols(spec.process(), 
    std::inserter(used_symbols, used_symbols.end()));

  // 2. Instantiate the mCRL2 selector 
  // This analyzes the dependency graph starting from your used_symbols
  mcrl2::data::used_data_equation_selector selector(
    spec.data(),
    used_symbols,
    std::set<mcrl2::data::variable>(), // Empty set of variables
    false                              // Do not eliminate variables
  );

  // 3. Create a new, filtered list of data equations
  std::vector<mcrl2::data::data_equation> to_remove;
  std::vector<mcrl2::data::function_symbol> to_remove_maps; 
  for (const auto& eqn : spec.data().user_defined_equations())
  {
    // The selector acts as a predicate function
    if (!selector(eqn))
    {
      to_remove.push_back(eqn);
    }
  }
  for (const auto& map : spec.data().user_defined_mappings())
  {
    if (!used_symbols.contains(map)) {
      std::cerr << "Removed " << map << std::endl;
      to_remove_maps.push_back(map);
    }
  }
  for (auto r : to_remove)
    spec.data().remove_equation(r);
  for (auto r : to_remove_maps)
    spec.data().remove_mapping(r);
}

int main() {
  const auto text = read_file("/home/milu/University/BFP/test/robot.mcrl2");

  mcrl2::lps::t_lin_options lin_options{};
  lin_options.ignore_time = true;
  lin_options.noglobalvars = true;
  lin_options.norewrite = true;
  lin_options.do_not_apply_constelm = true;
  lin_options.apply_alphabet_axioms = false;
  lin_options.binary = true;

  auto lin = mcrl2::lps::linearise(text, lin_options);

  auto spec = mcrl2::lps::remove_stochastic_operators(lin);

  // prune_unused_equations(spec);
  // mcrl2::data::rewriter rewriter(spec.data(), mcrl2::data::jitty);
  // mcrl2::lps::constelm(spec, rewriter, false);

  std::cerr << spec << std::endl;
  // return 0;

  auto config = YAML::LoadFile("../../test/config_new.yaml");
  for (auto b : config["bounds"]) {
    trans::gctx.bounds.insert_or_assign(b.first.as<std::string>(), b.second.as<std::vector<int>>());
  }

  for (auto a : config["actions"]) {
    std::vector<trans::Action::Parameter> parameters;

    for (auto p : a["parameters"]) {
      if (p["input"])
        parameters.push_back({p["input"].as<std::string>(), true,
          p["transform"] ? p["transform"].as<std::string>() : "",
        p["id"] ? p["id"].as<int>() : 0});
      else if (p["output"])
        parameters.push_back({p["output"].as<std::string>(), false,
          p["transform"] ? p["transform"].as<std::string>() : "",
        p["id"] ? p["id"].as<int>() : 0});
    }

    trans::gctx.actions.insert_or_assign(
      a["action_name"].as<std::string>(), 
      trans::Action{
        a["fb_name"] ? a["fb_name"].as<std::string>() : "",
        a["condition"] ? a["condition"].as<std::string>() : "",
        std::move(parameters)
      }
    );
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

  trans::gctx.symbs = {
    {"true", "TRUE"},
    {"false", "FALSE"},
    {"!", "NOT"},
    {"@c0", "0"},
    {"pair", "Position"}
  };
  
  auto sorts = std::vector<std::shared_ptr<emit::Type>> {};
  for (const auto& alias : spec.data().user_defined_aliases()) {
    sorts.push_back(trans::trans_sort(alias));
    if (auto et = dynamic_cast<emit::EnumType*>(sorts.back().get())) {
      for (size_t i = 0; const auto op : et->options)
        trans::gctx.symbs.insert_or_assign(op, 
          std::format("{}", i++));
    } else if (auto st = dynamic_cast<emit::StructType*>(sorts.back().get())) {
      for (const auto comp : st->components)
        trans::gctx.struct_comps.insert(comp.first);
    }
  }

  auto maps = trans::trans_maps(spec, {});
  auto proc = trans::trans_proc(spec.process(), spec.initial_process(), {});

  std::cerr << "Ensure the following sorts are defined:\n";
  for (const auto& s : sorts)
    std::cerr << *s;
  std::cerr << '\n';
  for (const auto& f : trans::gctx.aux_functions)
    std::cout << f << '\n';
  for (const auto& m : maps)
    std::cout << m << '\n';
  std::cout << proc << '\n';  
}
