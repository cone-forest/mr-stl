cmake_minimum_required(VERSION 3.27)

set(MR_STL_PROJECT_NAME mr-stl)
set(MR_STL_LIB_NAME     mr-stl-lib)
set(MR_STL_BENCH_NAME   mr-stl-bench)
set(MR_STL_TESTS_NAME   mr-stl-tests)

project(
  ${MR_STL_PROJECT_NAME}
  VERSION 1.0.0
  LANGUAGES CXX
)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(MR_STL_ENABLE_BENCHMARK "Whether benchmarks are downloaded and build"    OFF)
option(MR_STL_ENABLE_TESTING   "Whether tests are downloaded and build"         OFF)
option(MR_STL_PRESET_OPTIMIZED "Whether optimization flags are enabled at all"  OFF)
option(MR_STL_PRESET_BENCHMARK "Whether maximum optimization flags are enabled" OFF)
option(MR_STL_PRESET_PROFILING "Whether profiling flags are enabled"            OFF)

include(cmake/dependencies.cmake)

add_library(${MR_STL_LIB_NAME} INTERFACE
  include/mr-stl/algorithm/algorithm.hpp
  include/mr-stl/bigint/bigint.hpp
  include/mr-stl/graph/graph.hpp
  include/mr-stl/hashmap/hashmap.hpp
  include/mr-stl/ringbuf/static_ringbuf.hpp
  include/mr-stl/ringbuf/dynamic_ringbuf.hpp
  include/mr-stl/span/span.hpp
  include/mr-stl/string/string.hpp
  include/mr-stl/vector/amortized_vector.hpp
  include/mr-stl/vector/vector.hpp
  include/mr-stl/def.hpp
  include/mr-stl/mr-stl.hpp
)

target_include_directories(${MR_STL_LIB_NAME} INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
target_compile_features(${MR_STL_LIB_NAME} INTERFACE cxx_std_23)

if (MR_STL_ENABLE_BENCHMARK)
  add_executable(${MR_STL_BENCH_NAME} "bench/main.cpp")
  if (NOT MSVC)
    set(MR_STL_BENCH_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wall -Wextra")
    if (MR_STL_PRESET_OPTIMIZED OR MR_STL_PRESET_BENCHMARK)
      set(MR_STL_BENCH_CXX_FLAGS "${MR_STL_BENCH_CXX_FLAGS} -O3 -ffast-math")
    endif()
    if (MR_STL_PRESET_BENCHMARK)
      set(MR_STL_BENCH_CXX_FLAGS "${MR_STL_BENCH_CXX_FLAGS} -march=native")
    endif()
    if (MR_STL_PRESET_PROFILING)
      set(MR_STL_BENCH_CXX_FLAGS "${MR_STL_BENCH_CXX_FLAGS} -lprofiler -ltcmalloc")
    endif()
  endif()
  set_property(TARGET ${MR_STL_BENCH_NAME} PROPERTY COMPILE_FLAGS "${MR_STL_BENCH_CXX_FLAGS}")
  target_link_libraries(${MR_STL_BENCH_NAME} PRIVATE benchmark ${MR_STL_LIB_NAME})
endif()

if (MR_STL_ENABLE_TESTING)
  include(GoogleTest)
  add_executable(${MR_STL_TESTS_NAME} "tests/main.cpp")
  target_link_libraries(${MR_STL_TESTS_NAME} PRIVATE gtest_main ${MR_STL_LIB_NAME})
  gtest_discover_tests(${MR_STL_TESTS_NAME})
endif()

packageProject(
  NAME ${MR_STL_LIB_NAME}
  VERSION ${PROJECT_VERSION}
  BINARY_DIR ${PROJECT_BINARY_DIR}
  INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include
  INCLUDE_DESTINATION include/${PROJECT_NAME}-${PROJECT_VERSION}
  INCLUDE_HEADER_PATTERN "*.hpp"
  NAMESPACE mr
  COMPATIBILITY AnyNewerVersion # supported values: `AnyNewerVersion|SameMajorVersion|SameMinorVersion|ExactVersion`
  ARCH_INDEPENDENT YES
  CPACK NO
)
