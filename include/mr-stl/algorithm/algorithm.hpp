#pragma once

#include <algorithm>
#include <iterator>
#include <ranges>

namespace mr {
  // standard quick sort's partition function
  template <typename It>
    It partition(It begin, It end) {
      using std::swap;
      swap(begin[std::distance(begin, end) / 2], end[-1]);
      auto pivot = end - 1;
      auto i = begin;
      for (auto j = begin; j != pivot; ++j) {
        if (*j < *pivot) {
          std::swap(*i, *j);
          i++;
        }
      }
      swap(*i, *pivot);
      return i;
    }

  template <typename It>
    void insertion_sort(It begin, It end) {
      auto distance = std::distance(begin, end);

      for (std::size_t i = 1; i < distance; i++) {
        auto tmp = begin[i];
        std::size_t j = i - 1;
        while (j >= 0 && tmp < begin[j]) {
          begin[j + 1] = begin[j];
          j--;
        }
        begin[j + 1] = tmp;
      }
    }

  // quick sort falling back to insertion sort
  template <typename It>
    void sort(It begin, It end) {
      // maximum number of elements fitting in a cacheline
      static constexpr int max_batch_size =
        std::hardware_destructive_interference_size /
        sizeof(std::remove_cvref_t<decltype(*begin)>);

      // divide until batch_size > max_batch_size
      if (std::distance(begin, end) >= max_batch_size) {
        // quick sort if not fitting in a cacheline
        auto mid = partition(begin, end);
        mr::sort(begin, mid);
        mr::sort(mid+1, end);
      } else if (std::distance(begin, end) > 1) {
        // insertion_sort if fit in a cacheline
        insertion_sort(begin, end);
      }
    }

  // ranges version (could be surrounded with ifdef)
  void sort(std::ranges::range auto &range) {
    auto b = begin(range);
    auto e = end(range);

    mr::sort(b, e);
  }
}
