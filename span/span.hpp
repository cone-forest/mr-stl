#ifndef __span_hpp__
#define __span_hpp__

#include "../def.hpp"

template <typename T>
  using Span = std::span<T>;

template <typename T>
  struct OwningSpan {
    std::size_t _size = 0;
    std::unique_ptr<T> _data {};

    OwningSpan() noexcept = default;

    OwningSpan(const T *data, std::size_t size) noexcept :
      _data(std::make_unique<T>(size)), _size(size) {
        std::memcpy(_data.get(), data, _size * sizeof(T));
      }

    OwningSpan(T *data, std::size_t size) noexcept :
      _data(data), _size(size) {}

    template <typename ...Args>
      requires (std::is_convertible_v<T, Args> && ...)
      OwningSpan(Args... args) :
        _data(std::make_unique<T>(sizeof...(args))),
        _size(sizeof...(args)) {
          std::array<T, sizeof...(args)> tmp {args...};
          std::memcpy(_data.get(), tmp.data(), tmp.size() * sizeof(T));
        }

    OwningSpan(std::size_t size) noexcept {
      if (size == 0) [[unlikely]] {
        return;
      }

      _data = std::make_unique<T>(size);
      _size = size;
    }

    T* data() { return _data.get(); }
    const T* data() const { return _data.get(); }
    std::size_t size() const { return _size; }
    T &operator[](std::size_t i) { return _data.get()[i]; }

    OwningSpan(OwningSpan &&other) noexcept {
      if (this == &other ||
          this->_data == other._data) {
        return;
      }

      std::swap(_data, other._data);
      std::swap(_size, other._size);
    }

    OwningSpan & operator=(OwningSpan &&other) noexcept {
      if (this == &other ||
          this->_data == other._data) {
        return *this;
      }

      std::swap(_data, other._data);
      std::swap(_size, other._size);
      return *this;
    }

    bool operator<(const OwningSpan<T> &other) const noexcept {
      return std::memcmp(_data.get(), other._data.get(), std::min(_size, other._size)) < 0;
    }

    ~OwningSpan() noexcept = default;
  };

#endif // __span_hpp__
