#include "vector.hpp"

namespace mr {
  template <typename T>
  struct AmortizedVector {
    private:
    static inline constexpr std::size_t initial_capacity = 1024 / sizeof(T);

    std::size_t _size = 0;
    std::size_t _capacity = initial_capacity;

    // ~1024 byte buffer of type T
    T buf[initial_capacity];
    std::optional<Vector<T>> _data;

    public:
    template <typename ...Args>
      requires (std::is_constructible_v<T, Args...>)
      AmortizedVector & emplace_back(Args ...args) {
        // choose buffer to insert value into
        if (_size < initial_capacity) [[likely]] {
          buf[_size++] = {args...};
        } else {
          if (!_data) {
            _data = Vector<T>(32);
          }

          _data->emplace_back(args...);
          _size++;
          _capacity = initial_capacity + _data->capacity();
        }

        return *this;
      }

      // getters
      std::size_t size() const noexcept {return _size; }
      std::size_t capacity() const noexcept {return _capacity; }
      T& operator[](std::size_t i) {
        if (i < initial_capacity) [[likely]] {
          return buf[i];
        }
        return _data[i - initial_capacity];
      }
      T operator[](std::size_t i) const {
        if (i < initial_capacity) [[likely]] {
          return buf[i];
        }
        return _data[i - initial_capacity];
      }
  };
}
