#include "mr-stl/mr-stl.hpp"
#include <random>

#define ENABLE_TESTS
// #define ENABLE_BENCHMARK

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
BENCHMARK(BM_sort);
BENCHMARK(BM_sort);
BENCHMARK(BM_sort);
BENCHMARK(BM_sort);
BENCHMARK(BM_sort);
BENCHMARK(BM_sort);

BENCHMARK_MAIN();
#else
int main() {
  /*
  mr::Vector<int> vec {
    93, 2, 46, 41, 24, 15, 83, 19, 29, 73, 99, 92, 79, 23, 13, 34, 40, 5, 90, 91, 7, 80, 55, 43, 31, 48, 96, 33, 17, 97, 1, 56, 9, 76, 58, 59, 57, 11, 82, 32, 22, 71, 88, 68, 66, 63, 50, 72, 44, 77, 64, 69, 94, 36, 12, 87, 37, 18, 16, 49, 51, 78, 84, 62, 60, 47, 35, 21, 89, 98, 10, 65, 28, 45, 3, 14, 25, 39, 95, 20, 81, 54, 70, 74, 42, 100, 67, 8, 38, 30, 75, 86, 61, 4, 6, 26, 52, 27, 53, 85
  };
  mr::Vector<int> vec_res {
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100
  };
  mr::sort(vec);
  */
  auto sorted = [](mr::Range auto r) {
    auto tmp = r;
    mr::sort(tmp);
    // benchmark::DoNotOptimize(tmp);
  };
  std::random_device rd;
  std::mt19937 gen(rd());
  auto rand = [&gen](int min, int max) {
    std::uniform_int_distribution<> dis(min, max);
    return dis(gen);
  };
  int len = 1000;
  std::vector<float> vec;
  vec.resize(len);

  for (int i = 0; i < len; i++) {
    vec[i] = rand(0, 1000) / 1000.f;
  }

  sorted(vec);
}
#endif
