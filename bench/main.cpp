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
  int len = 1000000;
  std::vector<int> vec;
  vec.resize(len);

  for (int i = 0; i < len; i++) {
    vec[i] = rand(0, 1000000);
  }

  for (auto _ : state) {
    sorted(vec);
  }
}

BENCHMARK(BM_sort);

static void BM_counting_sort(benchmark::State& state) {
  auto sorted = [](mr::Range auto r) {
    auto tmp = r;
    mr::counting_sort(tmp.data(), tmp.size());
    benchmark::DoNotOptimize(tmp);
  };
  std::random_device rd;
  std::mt19937 gen(rd());
  auto rand = [&gen](int min, int max) {
    std::uniform_int_distribution<> dis(min, max);
    return dis(gen);
  };
  int len = 1000000;
  std::vector<uint32_t> vec;
  vec.resize(len);

  for (int i = 0; i < len; i++) {
    vec[i] = len + rand(0, len);
  }

  for (auto _ : state) {
    sorted(vec);
  }
}

BENCHMARK(BM_counting_sort);

static void BM_FindPath(benchmark::State &state) {
  mr::Graph<int> graph;
  const int num_nodes = state.range(0);

  // Create a linear chain of nodes
  for (int i = 0; i < num_nodes; ++i) {
    graph.add_node(i);
  }
  for (int i = 0; i < num_nodes - 1; ++i) {
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

#include <vector>
#include <random>
#include <algorithm>
#include <cassert>

// Insertion Benchmark
static void BM_Insert(benchmark::State& state) {
  const int N = state.range(0);
  std::mt19937 rng(N);
  std::uniform_int_distribution<int> dist;

  std::vector<int> data(N);
  for (auto& num : data) {
    num = dist(rng);
  }

  for (auto _ : state) {
    mr::SplayTree<int> tree;
    for (const auto num : data) {
      tree.insert(num);
    }
    benchmark::DoNotOptimize(tree);
  }
  state.SetComplexityN(N);
}
BENCHMARK(BM_Insert)->RangeMultiplier(2)->Range(8, 8<<10)->Complexity();

// Find Benchmark
static void BM_Find(benchmark::State& state) {
  const int N = state.range(0);
  std::mt19937 rng(N);
  std::uniform_int_distribution<int> udist;
  std::normal_distribution<double> ndist;

  if (N == 512) {
    std::println("life is shit");
  }

  std::vector<int> data(N);
  for (auto& num : data) {
    num = udist(rng);
  }

  std::vector<int> search_keys = data;
  for (auto& num : search_keys) {
    num = (int)(ndist(rng) * N);
  }

  for (auto _ : state) {
    state.PauseTiming();
    mr::SplayTree<int> tree;
    for (const auto num : data) {
      tree.insert(num);
    }
    state.ResumeTiming();

    for (const auto key : search_keys) {
      auto node = tree.find(key);
      benchmark::DoNotOptimize(node);
    }

    state.PauseTiming();
  }
  state.SetComplexityN(N);
}
BENCHMARK(BM_Find)->RangeMultiplier(2)->Range(8, 8<<10)->Complexity();

// Erase Benchmark
static void BM_Erase(benchmark::State& state) {
  const int N = state.range(0);
  std::mt19937 rng(N);
  std::uniform_int_distribution<int> dist;

  std::vector<int> data(N);
  for (auto& num : data) {
    num = dist(rng);
  }

  std::vector<int> erase_order = data;
  std::shuffle(erase_order.begin(), erase_order.end(), rng);

  for (auto _ : state) {
    state.PauseTiming();
    mr::SplayTree<int> tree;
    for (const auto num : data) {
      tree.insert(num);
    }
    state.ResumeTiming();

    for (const auto num : erase_order) {
      tree.erase(num);
    }

    state.PauseTiming();
    assert(tree.empty());
  }
  state.SetComplexityN(N);
}
BENCHMARK(BM_Erase)->RangeMultiplier(2)->Range(8, 8<<10)->Complexity();

// Run the benchmark
BENCHMARK_MAIN();
