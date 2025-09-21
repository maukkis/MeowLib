cmake_minimum_required(VERSION 3.25.1)

project(nyaBot
DESCRIPTION "simple discord bot library written in C++"
VERSION 0.2.0)
find_package(nlohmann_json REQUIRED)
find_package(Threads REQUIRED)
set(CMAKE_CXX_STANDARD 23)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_compile_options(-Wall -Werror -O1 -g)
else()
  add_compile_options(-Wall -Werror -O3)
endif()

file(GLOB_RECURSE nyaBot_src "src/*.cpp")
add_subdirectory(${PROJECT_SOURCE_DIR}/meowHttp)
add_library(nyaBot ${nyaBot_src})
target_link_libraries(nyaBot PRIVATE nlohmann_json::nlohmann_json meowHttp Threads::Threads)
