#include "mr-stl/mr-stl.hpp"
#include <random>

// #define ENABLE_TESTS
#define ENABLE_BENCHMARK

#ifdef ENABLE_TESTS
#include "gtest/gtest.h"
template <typename T>
struct MVector {
  std::mutex m;
  mr::Vector<T> vec;

  template <typename ...Ts>
  MVector & emplace_back(Ts ...args) {
    std::lock_guard lg(m);

    vec.emplace_back(args...);

    return *this;
  }

  std::size_t size() const noexcept { return vec.size(); }
};

template <typename T>
struct LFVector {
  std::atomic<int> af;
  mr::Vector<T> vec;

  template <typename ...Ts>
    LFVector & emplace_back(Ts ...args) {
      vec.emplace_back(args...);
      return *this;
    }

  std::size_t size() const noexcept { return vec.size(); }
};

int test_body(auto && vec) {
  constexpr int threads_num = 100;
  constexpr int thread_work = 100;
  std::thread threads[threads_num];

  for (int i = 0; i < threads_num; i++) {
    threads[i] = std::thread([i, &vec]() {
        for (int j = 0; j < thread_work; j++) {
          vec.emplace_back(j);
        }
        });
  }
  for (int i = 0; i < threads_num; i++) {
    threads[i].join();
  }

  return threads_num * thread_work;
}

TEST(MVectorTest, PushBackTest) {
  MVector<int> vec;
  auto size = test_body(vec);
  EXPECT_EQ(size, vec.size());
}

TEST(VectorSortTest, VectorSortTest) {
  mr::Vector<int> vec {
    93, 2, 46, 41, 24, 15, 83, 19, 29, 73, 99, 92, 79, 23, 13, 34, 40, 5, 90, 91, 7, 80, 55, 43, 31, 48, 96, 33, 17, 97, 1, 56, 9, 76, 58, 59, 57, 11, 82, 32, 22, 71, 88, 68, 66, 63, 50, 72, 44, 77, 64, 69, 94, 36, 12, 87, 37, 18, 16, 49, 51, 78, 84, 62, 60, 47, 35, 21, 89, 98, 10, 65, 28, 45, 3, 14, 25, 39, 95, 20, 81, 54, 70, 74, 42, 100, 67, 8, 38, 30, 75, 86, 61, 4, 6, 26, 52, 27, 53, 85
  };
  mr::Vector<int> vec_res {
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100
  };
  mr::sort(vec);
  EXPECT_EQ(vec, vec_res);
}

TEST(GraphTest, AddNodesAndEdges) {
    mr::Graph<int> graph;
    graph.add_node(0);
    graph.add_node(1);
    graph.add_edge(0, 1);

    auto children = graph.node_children(0);
    ASSERT_TRUE(children.has_value());
    ASSERT_EQ(children->size(), 1);
    EXPECT_EQ((*children)[0], 1);
}

TEST(GraphTest, FindPath) {
    mr::Graph<int> graph;
    graph.add_node(0);
    graph.add_node(1);
    graph.add_node(2);
    graph.add_edge(0, 1);
    graph.add_edge(1, 2);

    auto path = graph.find_path(0, 2);
    ASSERT_TRUE(path.has_value());
    ASSERT_EQ(path->size(), 3);
    EXPECT_EQ((*path)[0], 0);
    EXPECT_EQ((*path)[1], 1);
    EXPECT_EQ((*path)[2], 2);
}

TEST(GraphTest, NoPathExists) {
    mr::Graph<int> graph;
    graph.add_node(0);
    graph.add_node(1);

    auto path = graph.find_path(0, 1);
    EXPECT_FALSE(path.has_value()); // No edge added

    graph.add_edge(0, 1);
    path = graph.find_path(0, 1);
    ASSERT_TRUE(path.has_value());
    EXPECT_EQ(path->size(), 2);
}

TEST(GraphTest, PathToSelf) {
    mr::Graph<int> graph;
    graph.add_node(42);

    auto path = graph.find_path(0, 0);
    ASSERT_TRUE(path.has_value());
    ASSERT_EQ(path->size(), 1);
    EXPECT_EQ((*path)[0], 42);
}

TEST(GraphTest, MultipleEdges) {
    mr::Graph<int> graph;
    graph.add_node(0);
    graph.add_node(1);
    graph.add_node(2);
    graph.add_edge(0, 1);
    graph.add_edge(0, 2);

    auto children = graph.node_children(0);
    ASSERT_TRUE(children.has_value());
    ASSERT_EQ(children->size(), 2);
    EXPECT_EQ((*children)[0], 1);
    EXPECT_EQ((*children)[1], 2);
}

TEST(GraphTest, CyclicGraph) {
    mr::Graph<int> graph;
    graph.add_node(0);
    graph.add_node(1);
    graph.add_node(2);
    graph.add_edge(0, 1);
    graph.add_edge(1, 2);
    graph.add_edge(2, 0); // Creates a cycle

    auto path = graph.find_path(0, 2);
    ASSERT_TRUE(path.has_value());
    ASSERT_EQ(path->size(), 3);
    EXPECT_EQ((*path)[0], 0);
    EXPECT_EQ((*path)[1], 1);
    EXPECT_EQ((*path)[2], 2);
}

TEST(GraphTest, DisconnectedGraph) {
    mr::Graph<int> graph;
    graph.add_node(0);
    graph.add_node(1);
    graph.add_node(2);
    graph.add_node(3);
    graph.add_edge(0, 1);
    graph.add_edge(2, 3); // Disconnected component

    auto path = graph.find_path(0, 3);
    EXPECT_FALSE(path.has_value()); // No path exists
}

TEST(GraphTest, MultiplePaths) {
    mr::Graph<int> graph;
    graph.add_node(0);
    graph.add_node(1);
    graph.add_node(2);
    graph.add_node(3);
    graph.add_edge(0, 1);
    graph.add_edge(1, 3);
    graph.add_edge(0, 2);
    graph.add_edge(2, 3); // Two paths: 0->1->3 and 0->2->3

    auto path = graph.find_path(0, 3);
    ASSERT_TRUE(path.has_value());
    ASSERT_EQ(path->size(), 3); // Shortest path is 0->1->3 or 0->2->3
    EXPECT_EQ((*path)[0], 0);
    EXPECT_EQ((*path)[2], 3);
}

TEST(GraphTest, LargeGraph) {
    mr::Graph<int> graph;
    const std::size_t num_nodes = 100;
    for (std::size_t i = 0; i < num_nodes; ++i) {
        graph.add_node(i);
    }
    for (std::size_t i = 0; i < num_nodes - 1; ++i) {
        graph.add_edge(i, i + 1); // Linear chain
    }

    auto path = graph.find_path(0, num_nodes - 1);
    ASSERT_TRUE(path.has_value());
    ASSERT_EQ(path->size(), num_nodes);
    for (std::size_t i = 0; i < num_nodes; ++i) {
        EXPECT_EQ((*path)[i], i);
    }
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
#elif defined(ENABLE_BENCHMARK)
#include <benchmark/benchmark.h>

static void BM_sort(benchmark::State& state) {
  auto sorted = [](mr::Range auto r) {
    auto tmp = r;
    mr::sort(tmp);
    benchmark::DoNotOptimize(tmp);
  };
  std::random_device rd;
  std::mt19937 gen(rd());
  auto rand = [&gen](int min, int max) {
    std::uniform_int_distribution<> dis(min, max);
    return dis(gen);
  };
  int len = 1000;
  std::vector<int> vec;
  vec.resize(len);

  for (int i = 0; i < len; i++) {
    vec[i] = rand(0, 1000);
  }

  for (auto _ : state) {
    sorted(vec);
  }
}

BENCHMARK(BM_sort);

static void BM_FindPath(benchmark::State &state) {
    mr::Graph<int> graph;
    const std::size_t num_nodes = state.range(0);

    // Create a linear chain of nodes
    for (std::size_t i = 0; i < num_nodes; ++i) {
        graph.add_node(i);
    }
    for (std::size_t i = 0; i < num_nodes - 1; ++i) {
        graph.add_edge(i, i + 1);
    }

    // Benchmark the path-finding algorithm
    for (auto _ : state) {
        auto path = graph.find_path(0, num_nodes - 1);
        benchmark::DoNotOptimize(path);
    }
    state.SetComplexityN(num_nodes);
}

// Register the benchmark
BENCHMARK(BM_FindPath)
    ->RangeMultiplier(2)
    ->Range(8, 8 << 10) // Test with 8 to 8192 nodes
    ->Complexity();

// Run the benchmark
BENCHMARK_MAIN();
#endif
