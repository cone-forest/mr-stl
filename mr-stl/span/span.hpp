#pragma once

#include "mr-stl/def.hpp"

namespace mr {
  template <typename T>
    using Span = std::span<T>;

  template <typename T>
    struct OwningSpan : FlatRangeMethods<OwningSpan, T>,
                        RangeOutputOperators<OwningSpan, T> {
      std::size_t _capacity = 0;
      T* _data = nullptr;

      OwningSpan() noexcept = default;

      OwningSpan(const OwningSpan &other) noexcept {
        if (this == &other) {
          return;
        }

        if (_capacity < other._capacity) {
          delete[] _data;
          _data = new (std::nothrow) T[other._capacity];
          _capacity = other._capacity;
        }

        if (_data != nullptr) {
          std::memcpy(_data, other._data, _capacity * sizeof(T));
        }
      }

      OwningSpan & operator=(const OwningSpan &other) noexcept {
        if (this == &other) { return *this; }

        if (_capacity < other._capacity) {
          delete[] _data;
          _data = new (std::nothrow) T[other._capacity];
          _capacity = other._capacity;
        }

        if (_data != nullptr) {
          std::memcpy(_data, other._data, _capacity * sizeof(T));
        }

        return *this;
      }

      OwningSpan(OwningSpan &&other) noexcept {
        if (this == &other) { return; }

        delete[] _data;

        _data = std::move(other._data);
        _capacity = std::move(other._capacity);
        other._data = nullptr;
        other._capacity = 0;
      }

      OwningSpan & operator=(OwningSpan &&other) noexcept {
        if (this == &other) { return *this; }

        delete[] _data;

        _data = std::move(other._data);
        _capacity = std::move(other._capacity);
        other._data = nullptr;
        other._capacity = 0;
        return *this;
      }

      ~OwningSpan() noexcept { delete[] _data; }

      OwningSpan(const T *data, std::size_t size) noexcept :
        _data(new (std::nothrow) T[size]), _capacity(size) {
          std::memcpy(_data, data, _capacity * sizeof(T));
        }
      OwningSpan(T *data, std::size_t size) noexcept :
        _data(new (std::nothrow) T[size]), _capacity(size) {
          std::memcpy(_data, data, _capacity * sizeof(T));
        }

      template <typename ...Args>
        requires (std::is_convertible_v<T, Args> && ...)
        OwningSpan(Args... args) :
          _data(new (std::nothrow) T[sizeof...(args)]),
          _capacity(sizeof...(args)) {
            std::array<T, sizeof...(args)> tmp {args...};
            std::memcpy(_data, tmp.data(), tmp.size() * sizeof(T));
          }

      OwningSpan(std::size_t size) noexcept {
        if (size == 0) [[unlikely]] {
          return;
        }

        if (_data = new (std::nothrow) T[size];
            _data != nullptr) {
          _capacity = size;
        }
      }

      T * data() noexcept { return _data; }
      const T * data() const noexcept { return _data; }

      std::size_t size() const noexcept { return _capacity; }

      T & operator[](std::size_t i) { return _data[i]; }
      const T & operator[](std::size_t i) const { return _data[i]; }

      bool operator<(const OwningSpan<T> &other) const noexcept {
        return std::memcmp(_data, other._data, std::min(_capacity, other._capacity)) < 0;
      }
    };
}
