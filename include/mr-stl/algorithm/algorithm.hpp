#pragma once

#include <bit>
#include <cstring>
#include <memory>

#include <algorithm>
#include <iterator>
#include <ranges>

namespace mr {
  inline void sort5(int arr[5]) {
    if (arr[0] > arr[1]) std::swap(arr[0], arr[1]);
    if (arr[2] > arr[3]) std::swap(arr[2], arr[3]);
    if (arr[0] > arr[2]) std::swap(arr[0], arr[2]);
    if (arr[1] > arr[3]) std::swap(arr[1], arr[3]);
    if (arr[1] > arr[2]) std::swap(arr[1], arr[2]);
    if (arr[0] > arr[4]) std::swap(arr[0], arr[4]);
    if (arr[1] > arr[4]) std::swap(arr[1], arr[4]);
    if (arr[2] > arr[4]) std::swap(arr[2], arr[4]);
    if (arr[3] > arr[4]) std::swap(arr[3], arr[4]);
    if (arr[1] > arr[2]) std::swap(arr[1], arr[2]);
    if (arr[2] > arr[3]) std::swap(arr[2], arr[3]);
  }

  // standard quick sort's partition function
  template <typename T>
  inline int partition(T* arr, int low, int high) {
    int mid = low + (high - low) / 2;
    int pivot_candidates[5] = {
      arr[low],
      arr[low + (high - low) / 4],
      arr[mid],
      arr[high - (high - low) / 4],
      arr[high]
    };
    sort5(pivot_candidates);
    int pivot = pivot_candidates[2];
    for (int i = low; i <= high; ++i) {
      if (arr[i] == pivot) {
        std::swap(arr[i], arr[high]);
        break;
      }
    }
    int i = low - 1;
    for (int j = low; j < high; ++j) {
      if (arr[j] < pivot) {
        ++i;
        std::swap(arr[i], arr[j]);
      }
    }
    std::swap(arr[i + 1], arr[high]);
    return i + 1;
  }

  template <typename It>
  int partition(It begin, It end) {
    return partition(begin, 0, end - begin);
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
        auto mid = begin + partition(begin, end);
        mr::sort(begin, mid);
        mr::sort(mid+1, end);
      } else if (std::distance(begin, end) > 1) {
        // insertion_sort if fit in a cacheline
        insertion_sort(begin, end);
      }
    }

  template <std::unsigned_integral T>
  void counting_sort(T* ptr, int size) {
    if (size <= 1) return; // Already sorted if 0 or 1 elements

    // Find maximum value to determine number of bits needed
    T max = *std::max_element(ptr, ptr + size);
    if (max == 0) return; // All elements are zero

    // Calculate number of bits required
    int bits = std::bit_width(max);

    // Temporary buffer for sorting
    std::unique_ptr<T[]> temp = std::make_unique<T[]>(size);

    for (int bit = 0; bit < bits; ++bit) {
        size_t count[2] = {0};

        // Count elements where the current bit is 0
        for (int i = 0; i < size; ++i) {
            bool current_bit = (ptr[i] >> bit) & 1;
            count[current_bit]++;
        }

        // Calculate starting positions (shift count[0] elements with bit 0)
        size_t positions[2] = {0, count[0]};

        // Distribute elements into temporary buffer
        for (int i = 0; i < size; ++i) {
            bool current_bit = (ptr[i] >> bit) & 1;
            temp[positions[current_bit]++] = ptr[i];
        }

        // Copy back to original array
        std::memcpy(ptr, temp.get(), size * sizeof(T));
    }
}

  // ranges version (could be surrounded with ifdef)
  void sort(std::ranges::range auto &range) {
    auto b = begin(range);
    auto e = end(range);

    auto bptr = &*b;

    mr::sort(bptr, bptr + std::distance(b, e));
  }
}
