#include "translate.hpp"
#include "emit/enum_type.hpp"
#include "emit/struct_type.hpp"
#include "emit/reference_type.hpp"
#include "emit/array_type.hpp"
#include "emit/reference.hpp"
#include <memory>
#include <format>

using namespace trans;

std::shared_ptr<emit::Type> trans::trans_sort(const mcrl2::data::alias &alias)
{
  auto name = std::string{alias.name().name()};

  if (mcrl2::data::is_structured_sort(alias.reference()))
  {
    auto stru = mcrl2::data::structured_sort{alias.reference()};

    if (stru.constructors().size() == 1 && stru.constructors().front().arguments().size() >= 1)
    {
      // Is a struct type.
      auto con = stru.constructors().front();
      auto components = emit::StructType::Components{};
      for (const auto &arg : con.arguments())
      {
        auto comp_name = arg.name();
        auto comp_type = mcrl2::data::basic_sort(arg.sort()).name();
        components.emplace_back(comp_name,
                                gctx.types.at(comp_type));
      }
      return std::make_shared<emit::StructType>(name, components);
    }
    else
    {
      // Is an enum type.
      auto options = std::vector<std::string>{};
      for (const auto &con : stru.constructors())
      {
        assert(con.arguments().size() == 0 && "Constructors with arguments not implemented");
        options.push_back(con.name());

        auto recog = con.recogniser().function().name();
        if (recog.size() > 0)
          gctx.recognizers.insert_or_assign(
            recog, std::make_shared<emit::Reference>(std::format("{}.{}", name, std::string(con.name())))
          );
      }
      return std::make_shared<emit::EnumType>(name, options);
    }
  }
  else if (mcrl2::data::is_function_sort(alias.reference()))
  {
    auto func = mcrl2::data::function_sort(alias.reference());
    assert(func.domain().size() == 1);
    assert(mcrl2::data::is_basic_sort(func.domain().front()));
    assert(mcrl2::data::is_basic_sort(func.codomain()));

    std::cout << name << " - " << func.domain() << " - " << func.codomain() << '\n';
    auto domain = mcrl2::data::basic_sort(func.domain().front());
    auto range = mcrl2::data::basic_sort(func.codomain());

    return std::make_shared<emit::ArrayType>(name, gctx.types.at(domain.name()), gctx.types.at(range.name()));
  }
  else
  {
    throw std::runtime_error(std::format("Unsupported sort type: {} in \"{}\"",
                                         alias.reference().function().name(), mcrl2::data::pp(alias)));
  }
}
