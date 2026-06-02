cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 20)
if(POLICY CMP0167)
  cmake_policy(SET CMP0167 OLD)
endif()

project(mcrl22plc LANGUAGES CXX)

find_package(Boost REQUIRED)
find_package(yaml-cpp REQUIRED)

add_executable(mcrl22plc
  src/main.cpp

  src/emit/application.cpp
  src/emit/array_type.cpp
  src/emit/assignment.cpp
  src/emit/binary.cpp
  src/emit/enum_type.cpp
  src/emit/fb_call.cpp
  src/emit/function.cpp
  src/emit/function_block.cpp
  src/emit/indentation.cpp
  src/emit/index.cpp
  src/emit/iterate.cpp
  src/emit/reference.cpp
  src/emit/selection.cpp
  src/emit/struct_type.cpp
  src/emit/type.cpp
  src/emit/variable.cpp

  src/trans/appl.cpp
  src/trans/expr.cpp
  src/trans/maps.cpp
  src/trans/proc.cpp
  src/trans/sort.cpp
  src/trans/ternary.cpp
)
target_include_directories(mcrl22plc PRIVATE
  include
)
target_link_libraries(mcrl22plc PRIVATE
  mcrl2_lps
  mcrl2_core
  mcrl2_data
  mcrl2_process
  mcrl2_atermpp
  mcrl2_smt
  mcrl2_symbolic
  mcrl2_utilities
  Boost::boost
  yaml-cpp::yaml-cpp
)
