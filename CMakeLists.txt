cmake_minimum_required(VERSION 3.25.1)

project(MeowLib
DESCRIPTION "simple discord bot library written in C++"
VERSION 0.6.0)
if(CMAKE_SYSTEM_NAME STREQUAL "AIX")
  add_compile_options(-pthread)
endif()
find_package(nlohmann_json REQUIRED)
find_package(Threads REQUIRED)
find_package(meowHttp REQUIRED)
set(CMAKE_CXX_STANDARD 23)
option(VOICE_ENABLED "enables voice support" OFF)
set(srcdir "${PROJECT_SOURCE_DIR}/src")
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
if(WIN32 AND MSVC)
  # msvc gives stupid warnings that dont really matter and causes this to not compile fuck msvc
  set(CMAKE_CXX_FLAGS_DEBUG "/DEBUG")
  set(CMAKE_CXX_FLAGS_RELEASE "/04")
else()
  set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra -Werror -fsanitize=address -O0 -DJSON_DIAGNOSTICS")
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_link_options(-fsanitize=address)
  endif()
  set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wall -Wextra -Werror -DNDEBUG")
endif()

if(VOICE_ENABLED)
  find_package(MLSPP REQUIRED)
  set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
  find_package(Opus REQUIRED)
  file(GLOB MeowLib_src "${srcdir}/*.cpp" "${srcdir}/dispatchHandlers/*.cpp" "${srcdir}/componentsv2/*.cpp" "${srcdir}/voice/*.cpp" "${srcdir}/voice/dave/*.cpp")
else()
  file(GLOB MeowLib_src "${srcdir}/*.cpp" "${srcdir}/dispatchHandlers/*.cpp" "${srcdir}/componentsv2/*.cpp")
endif()
add_library(MeowLib ${MeowLib_src})

# discord requires us to specify what os we use
target_compile_definitions(MeowLib PRIVATE OS="${CMAKE_SYSTEM_NAME}")


target_compile_definitions(MeowLib PRIVATE MEOWLIB_VERSION="${PROJECT_VERSION}")
target_link_libraries(MeowLib PUBLIC nlohmann_json::nlohmann_json meowHttp Threads::Threads)

target_include_directories(MeowLib PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)

install(TARGETS MeowLib EXPORT MeowLibTargets DESTINATION lib)
if(VOICE_ENABLED)
  target_include_directories(MeowLib PUBLIC ${OPUS_INCLUDE_DIR})
  target_include_directories(MeowLib PUBLIC ${MLSPP_DIR}/../../include/mlspp)
  target_compile_definitions(MeowLib PRIVATE VOICE_ENABLED)
  target_link_libraries(MeowLib PUBLIC ${OPUS_LIBRARY} mlspp)
  install(DIRECTORY ${PROJECT_SOURCE_DIR}/include// DESTINATION include/MeowLib)
else()
  install(DIRECTORY ${PROJECT_SOURCE_DIR}/include// DESTINATION include/MeowLib PATTERN "voice/*" EXCLUDE)
endif()
include(CMakePackageConfigHelpers)
set(config_dir lib/cmake/MeowLib)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/MeowLib/MeowLibConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/MeowLib/MeowLibConfig.cmake"
  INSTALL_DESTINATION ${config_dir}
  NO_SET_AND_CHECK_MACRO
  NO_CHECK_REQUIRED_COMPONENTS_MACRO
)

export(
  EXPORT MeowLibTargets
  FILE "${CMAKE_CURRENT_BINARY_DIR}/MeowLib/MeowLibTargets.cmake"
)

install(
  FILES
    ${CMAKE_CURRENT_BINARY_DIR}/MeowLib/MeowLibConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/MeowLib/MeowLibConfigVersion.cmake
  DESTINATION ${config_dir}
)

if(VOICE_ENABLED)
  install(
    FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/cmake/FindOpus.cmake
    DESTINATION ${config_dir}
  )
endif()

install(EXPORT MeowLibTargets
  FILE
    MeowLibTargets.cmake
  DESTINATION
    ${config_dir}
)
