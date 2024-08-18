#pragma once

#include <array>
#include <optional>

namespace mr {
  template <typename T, std::size_t S>
    struct StaticRingBuffer {
    private:
      std::array<T, S> _data;
      std::size_t _size;
      std::size_t _head;
      std::size_t _tail;

    public:
      // default constructor
      constexpr StaticRingBuffer() noexcept = default;
      // copy constructor
      constexpr StaticRingBuffer(const StaticRingBuffer &) noexcept = default;
      // move constructor
      constexpr StaticRingBuffer(StaticRingBuffer &&) noexcept = default;
      // copy assignment operator
      constexpr StaticRingBuffer &operator=(const StaticRingBuffer &) noexcept =
          default;
      // move assignment operator
      constexpr StaticRingBuffer &operator=(StaticRingBuffer &&) noexcept =
          default;
      // destructor
      ~StaticRingBuffer() noexcept = default;

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
