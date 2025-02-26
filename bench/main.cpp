#include <random>

#include <benchmark/benchmark.h>

#include <mr-stl/mr-stl.hpp>

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
