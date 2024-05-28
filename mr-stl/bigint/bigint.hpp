#include <cassert>
#include <cstdint>
#include <limits>
#include "mr-stl/vector/vector.hpp"

namespace mr {
  template <std::integral T = std::uint64_t>
  struct UnsignedBigInt {
    inline static constexpr T shit_max = std::numeric_limits<T>::max();
    enum class Sign : int {
      Negative = 0,
      Positive = 1,
    };

    // absolute value
    Vector<T> _value;
    // sign
    Sign _sign = Sign::Positive;

    UnsignedBigInt() noexcept = default;
    UnsignedBigInt(Vector<T> value, Sign sign = Sign::Positive) noexcept :
      _value(std::move(value)), _sign(sign) {}

    UnsignedBigInt(UnsignedBigInt &&other) noexcept = default;
    UnsignedBigInt & operator=(UnsignedBigInt &&other) noexcept = default;

    UnsignedBigInt(const UnsignedBigInt &other) noexcept = default;
    UnsignedBigInt & operator=(const UnsignedBigInt &other) noexcept = default;

    template <std::integral S> UnsignedBigInt(S value) { _value.emplace_back(static_cast<T>(value)); }

    friend UnsignedBigInt operator+(const UnsignedBigInt &lhs, const UnsignedBigInt &rhs);
    friend UnsignedBigInt operator-(const UnsignedBigInt &lhs, const UnsignedBigInt &rhs);
    friend UnsignedBigInt operator+(const UnsignedBigInt &rhs);
    friend UnsignedBigInt operator-(const UnsignedBigInt &rhs);
    friend UnsignedBigInt operator*(const UnsignedBigInt &lhs, const UnsignedBigInt &rhs);
    friend UnsignedBigInt operator/(const UnsignedBigInt &lhs, const UnsignedBigInt &rhs);
    friend UnsignedBigInt operator*(const UnsignedBigInt &lhs, T rhs);
    friend UnsignedBigInt operator/(const UnsignedBigInt &lhs, T rhs);
    friend UnsignedBigInt operator%(const UnsignedBigInt &lhs, const UnsignedBigInt &rhs);

    T operator[](std::size_t i) const { return _value[i]; }
    T & operator[](std::size_t i) { return _value[i]; }
    auto operator<(const UnsignedBigInt &other) const noexcept {
      return _sign == Sign::Negative && other._sign == Sign::Positive ||
        (_sign == Sign::Positive ? (_value < other._value) : !(_value < other._value));
    }
  };

  // addition operators definitions
  template <std::integral T>
  UnsignedBigInt<T> operator+(const UnsignedBigInt<T> &lhs, const UnsignedBigInt<T> &rhs) {
    // swap arguments if len(lhs) < len(rhs)
    if (lhs._value.size() < rhs._value.size()) {
      return rhs + lhs;
    }
    // continue assuming len(lhs) >= len(rhs)
    // perform addition
    UnsignedBigInt tmp = lhs;
    bool trail = false;
    for (std::size_t i = 0; i < rhs._value.size(); i++) {
      bool next_trail = (tmp._value[i] > UnsignedBigInt<T>::shit_max - std::max<T>(rhs._value[i], trail));
      tmp._value[i] += rhs._value[i] + static_cast<T>(trail);
      trail = next_trail;
    }

    // handle trailing 1
    if (trail) {
      if (lhs._value.size() != rhs._value.size()) {
        tmp[rhs._value.size()] += trail;
      } else {
        tmp._value.emplace_back(trail);
      }
    }

    return tmp;
  }

  template <std::integral T>
  UnsignedBigInt<T> operator-(const UnsignedBigInt<T> &lhs, const UnsignedBigInt<T> &rhs) {
    // swap arguments if len(lhs) < len(rhs)
    if (lhs < rhs) {
      return -(rhs - lhs);
    }

    // continue assuming len(lhs) >= len(rhs)
    // perform addition
    UnsignedBigInt tmp = lhs;
    bool trail = false;
    for (std::size_t i = 0; i < rhs._value.size(); i++) {
      bool next_trail = (tmp._value[i] < std::max<int>(rhs._value[i], rhs._value[i] + trail));
      tmp._value[i] -= rhs._value[i] + static_cast<T>(trail);
      trail = next_trail;
    }

    return tmp;
  }

  template <std::integral T>
  UnsignedBigInt<T> operator+(const UnsignedBigInt<T> &rhs) {
    auto tmp = rhs;
    return tmp;
  };

  template <std::integral T>
  UnsignedBigInt<T> operator-(const UnsignedBigInt<T> &rhs) {
    auto tmp = rhs;
    tmp.sign = UnsignedBigInt<T>::Sign(!static_cast<int>(tmp.sign));
    return tmp;
  };

  template <std::integral T>
  UnsignedBigInt<T> operator*(const UnsignedBigInt<T> &lhs, const UnsignedBigInt<T> &rhs) {
    auto tmp = lhs;
    return tmp;
  }

  template <std::integral T>
  UnsignedBigInt<T> operator*(const UnsignedBigInt<T> &lhs, T rhs) {
    auto size = lhs.size() + rhs.size();
    UnsignedBigInt<T> tmp(size, 0);
    for (int i = 0; i < lhs.size(); i++) {
        T carry = 0;
        for (std::size_t j = 0; j < rhs.size() || carry; j++) {
            T product = tmp[i + j] + lhs[i] * (j < rhs.size() ? rhs[j] : (T)0) + carry;
            tmp[i + j] = product % (T)10;
            carry = product / (T)10;
        }
    }
    // remove trailing 0
    while (tmp[size - 1] == 0) {
      size--;
    }
    return {{tmp._value.data(), size}};
  }

  template <std::integral T>
  std::tuple<UnsignedBigInt<T>, T> operator/(const UnsignedBigInt<T> &lhs, T rhs) {
    auto size = lhs.size();
    T first = lhs[0];
    UnsignedBigInt<T> result(size);
    T remainder = 0;
    for (int i = lhs.size() - 1; i >= 0; i--) {
        T val = (remainder * (UnsignedBigInt<T>::shit_max / rhs) + lhs[i] / rhs);
        remainder = remainder * (UnsignedBigInt<T>::shit_max % rhs) + lhs[i] % rhs;
        result[i] = val;
    }
    if (first == rhs) {
      size--;
    }
    return {{result.data(), size}, remainder};
  }
}
