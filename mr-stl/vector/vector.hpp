#pragma once

#include "mr-stl/def.hpp"
#include "mr-stl/span/span.hpp"

namespace mr {
  template <typename T>
    struct Vector : FlatRangeMethods<Vector, T>,
                    RangeOutputOperators<Vector, T> {
      std::size_t _size = 0;
      OwningSpan<T> _data = {};

      Vector() noexcept = default;
      ~Vector() noexcept = default;

      template <typename ...Args>
        // requires (std::is_constructible_v<T, Args> && ...)
        Vector(Args... args) : _data(static_cast<T>(args)...), _size(sizeof...(args)) {}

      Vector(std::size_t size) :
        _data(size) {}

      Vector(const T *data, std::size_t size) :
        _data(data, size), _size(size) {}

      // copy semantic
      Vector(const Vector &other) noexcept = default;
      Vector & operator=(const Vector &other) noexcept = default;

      // move semantic
      Vector(Vector &&other) noexcept = default;
      Vector & operator=(Vector &&other) noexcept = default;

      template <typename ...Args>
        // requires (std::is_constructible_v<T, Args...>)
        Vector & emplace_back(Args ...args) {
          while (_size >= _data.size()) [[unlikely]] {
            try_resize();
          }
          T tmp = {std::forward<Args>(args)...};
          _data[_size++] = std::move(tmp);
          return *this;
        }

      template <std::input_iterator It>
        Vector & move_range(It begin, It end) {
          std::size_t new_size = std::distance(begin, end);
          while (_data.size() < new_size) {
            try_resize();
          }
          std::uninitialized_move(begin, end, _data.data());
          return *this;
        }

      template <typename ...Args>
        requires (std::is_constructible_v<T, Args...>)
        Vector & push_sorted(Args ...args) {
          emplace_back(args...);
          // insertion sort
          std::size_t i = _size - 1; // last element index
          auto at = [this](auto idx) -> T& { return (*this)[idx]; };

          T val = at(i);
          while (i > 0 && at(i - 1) > val) {
            at(i) = std::move(at(i - 1));
            i--;
          }
          at(i) = std::move(val);

          return *this;
        }

      template <typename ...Args>
        requires (std::is_constructible_v<T, Args...>)
      Vector & emplace_at(std::size_t index, Args ...args) {
        emplace_back();
        std::memmove(_data + index, _data + index + 1, size() - index - 1);
        _data[index] = std::move(T(args...));
        return *this;
      }

      template <typename ...Args>
        requires (std::is_constructible_v<T, Args...>)
      Vector & emplace_at(T *location, Args ...args) {
        return emplace_at(std::distance(data(), location), args...);
      }

      Vector & remove(std::size_t id) {
        _size--;
        for (std::size_t i = id; i < _size; i++) {
          _data[i] = _data[i + 1];
        }
      }

      Vector& resize(std::size_t new_size) {
        while (_data.size() < new_size) [[unlikely]] {
          try_resize();
        }
      }

      Vector & clear() {
        _size = 0;
      }

      // getters
      T * data() noexcept { return _data.data(); }
      const T * data() const noexcept { return _data.data(); }

      std::size_t size() const noexcept {return _size; }
      std::size_t capacity() const noexcept {return _data.size(); }

      T& operator[](std::size_t i) { return _data[i]; }
      const T & operator[](std::size_t i) const { return _data[i]; }

      bool operator<(const Vector<T> &other) const noexcept {
        return _size < other._size || (_size == other._size && _data < other._data);
      }

      bool operator==(const Vector<T> &other) const noexcept {
        return !(*this < other) && !(other < *this);
      }

    private:
      std::optional<OwningSpan<T>> resized(const std::size_t size) {
        if (T *tmp = new (std::nothrow) T[size]; tmp != nullptr) [[likely]] {
          // move on successful allocation
          std::uninitialized_move_n(_data.data(), _data.size(), tmp);
          return OwningSpan(tmp, size);
        }
        return std::nullopt;
      }

      void try_resize() {
        _data = resized(_size * 2 + 1).value_or(std::move(_data)); // assign new value on success
      }
    };
}
