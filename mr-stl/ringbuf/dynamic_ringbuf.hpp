#pragma once

#include "mr-stl/vector/vector.hpp"

namespace mr {
  template <typename T>
    struct DynamicRingBuffer {
    private:
      mr::Vector<T> _data;
      std::size_t _size = 0;
      int _head = 0;
      int _tail = 0;

    public:
      // default constructor
      constexpr DynamicRingBuffer() noexcept = default;
      // copy constructor
      constexpr DynamicRingBuffer(const DynamicRingBuffer &) noexcept = default;
      // move constructor
      constexpr DynamicRingBuffer(DynamicRingBuffer &&) noexcept = default;
      // copy assignment operator
      constexpr DynamicRingBuffer &operator=(const DynamicRingBuffer &) noexcept = default;
      // move assignment operator
      constexpr DynamicRingBuffer &operator=(DynamicRingBuffer &&) noexcept = default;
      // destructor
      ~DynamicRingBuffer() noexcept = default;

      constexpr bool push_back(T value) {
        if (full()) {
          resize(_head + _size + 3);
        }

        _data[_tail] = std::move(value);
        _tail = (_tail + 1) % capacity();
        _size++;

        return true;
      }

      constexpr bool push_front(T value) {
        if (full()) {
          resize(_head + _size);
        }

        _head = (_head + capacity() - 1) % capacity();
        _data[_head] = std::move(value);
        _size++;

        return true;
      }

      constexpr std::optional<T> pop_front() noexcept {
        if (empty()) {
          return std::nullopt;
        }

        _head = (_head + 1) % size();
        _size--;

        return std::move(_data[(_head + capacity() - 1) % capacity()]);
      }

      constexpr std::optional<T> pop_back() noexcept {
        if (empty()) {
          return std::nullopt;
        }

        _tail = (_tail + capacity() - 1) % capacity();
        _size--;

        return std::move(_data[_tail]);
      }

      const T & operator[](std::size_t index) const {
        return _data[(_head + index) % capacity()];
      }

      T & operator[](std::size_t index) {
        return _data[(_head + index) % capacity()];
      }

      std::optional<T> at(std::size_t index) const noexcept {
        if (_size != 0) [[likely]] {
          return _data[(_head + index) % capacity()];
        }
        return std::nullopt;
      }

      constexpr std::size_t size() const noexcept { return _size; }

      constexpr std::size_t capacity() const noexcept { return _data.size(); }

      constexpr bool empty() const noexcept { return _size == 0; }

      constexpr bool full() const noexcept { return _size == capacity(); }

      constexpr size_t head() const noexcept { return _head; }

      constexpr size_t tail() const noexcept { return _tail; }

      void resize(std::size_t new_size) {
        _data.resize(new_size);
        // NOTE: it's probably not worth it to move all data to the beginning
        std::uninitialized_move_n(begin(_data), _head < _tail ? 0 : _tail,
                                  begin(_data) + _head + _size - _tail);
        _tail = _head + _size;
      }
};
}  // namespace mr
