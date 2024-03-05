#ifndef __vector_hpp__
#define __vector_hpp__

#include "../def.hpp"
#include "../span/span.hpp"

template <typename T>
  struct Vector {
    std::size_t _size = 0;
    OwningSpan<T> _data = {};

    Vector() noexcept = default;

    template <typename ...Args> requires (std::is_constructible_v<T, Args> && ...)
      Vector(Args... args) : _data(static_cast<T>(args)...), _size(sizeof...(args)) {}

    Vector(const T *data, std::size_t size) :
      _data(data, size), _size(size) {}

    template <typename ...Args>
      requires (std::is_constructible_v<T, Args...>)
    Vector & emplace_back(Args ...args) {
      if (_size >= _data.size()) [[unlikely]] {
        _data = resized(_size * 2 + 1).value_or(std::move(_data)); // assign new value on success
      }
      _data[_size++] = {args...};
      return *this;
    }

    std::optional<OwningSpan<T>> resized(const std::size_t size) {
      if (T *tmp = new (std::nothrow) T[size]; tmp != nullptr) [[likely]] {
        // copy on successful allocation
        std::memcpy(tmp, _data.data(), _size * sizeof(T));
        return OwningSpan(tmp, size);
      }

      return std::nullopt;
    }

    T * begin() {
      return _data.data();
    }
    T * end() {
      return _data.data() + _size;
    }

    std::size_t size() const {return _size; }
    std::size_t capacity() const {return _data.size(); }
    T& operator[](std::size_t i) { return _data[i]; }
  };

#endif // __vector_hpp__
