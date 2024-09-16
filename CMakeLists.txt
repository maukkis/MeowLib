cmake_minimum_required(VERSION 3.25.1)

project(nyaBot
DESCRIPTION "simple discord bot written in C++")
find_package(CURL REQUIRED)
find_package(nlohmann_json REQUIRED)
set(CMAKE_CXX_STANDARD 23)
set(DCKMAKE_BUILD_TYPE Release)
file(GLOB_RECURSE nyaBot_src "src/*.cpp"
"src/*.h")

add_executable(nyaBot ${nyaBot_src})
target_link_libraries(nyaBot PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(nyaBot PRIVATE ${CURL_LIBRARIES})
target_include_directories(nyaBot PRIVATE ${CURL_INCLUDE_DIRS})
