#ifndef __mr_stl_hpp__
#define __mr_stl_hpp__

#include "def.hpp"
#if __cpp_lib_span >= 202002L
#  include "span/span.hpp"
#endif
#include "vector/vector.hpp"
// #include "vector/amortized_vector.hpp"
#include "string/string.hpp"
#include "hashmap/hashmap.hpp"
#include "graph/graph.hpp"
#include "algorithm/algorithm.hpp"

#endif // __mr_stl_hpp__
