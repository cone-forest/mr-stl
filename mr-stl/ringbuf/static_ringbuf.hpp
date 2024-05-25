#pragma once

#include <array>
#include <optional>

namespace mr {
  template <typename T, std::size_t S>
    struct DynamicRingBuffer {
    private:
      std::array<T, S> _data;
      std::size_t _size;
      std::size_t _head;
      std::size_t _tail;

    public:
      // default constructor
      constexpr DynamicRingBuffer() noexcept = default;
      // copy constructor
      constexpr DynamicRingBuffer(const DynamicRingBuffer &) noexcept = default;
      // move constructor
      constexpr DynamicRingBuffer(DynamicRingBuffer &&) noexcept = default;
      // copy assignment operator
      constexpr DynamicRingBuffer &operator=(const DynamicRingBuffer &) noexcept =
          default;
      // move assignment operator
      constexpr DynamicRingBuffer &operator=(DynamicRingBuffer &&) noexcept =
          default;
      // destructor
      ~DynamicRingBuffer() noexcept = default;

      bool push(T value) {
        if (full()) {
          return false;
        }

        _data[_tail] = value;
        _tail = (_tail + 1) % _data.size();
        _size++;

        return true;
      }

      constexpr std::optional<T> pop() noexcept {
        if (empty()) {
          return std::nullopt;
        }

        T value = _data[_head];
        _head = (_head + 1) % _data.size();
        _size--;

        return value;
      }

      constexpr size_t size() const { return _size; }

      constexpr size_t capacity() const { return S; }

      constexpr bool empty() const { return _size == 0; }

      constexpr bool full() const { return _size == S; }
    };
}  // namespace mr
