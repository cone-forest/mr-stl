#include <cassert>
#include <cstdint>
#include <limits>

#include "mr-stl/string/string.hpp"
#include "mr-stl/vector/vector.hpp"

namespace mr {
  template <std::integral T = std::uint64_t>
  struct BigInt {
    inline static constexpr T shit_max = std::numeric_limits<T>::max();
    enum class Sign : int {
      Negative = 0,
      Positive = 1,
    };

    // absolute value
    Vector<T> _value;
    // sign
    Sign _sign = Sign::Positive;

    BigInt() noexcept = default;
    BigInt(Vector<T> value, Sign sign = Sign::Positive) noexcept : _value(std::move(value)), _sign(sign) {}
    BigInt(mr::StringView<char> init) {
      if (!validate_init_value(init)) {
        throw std::invalid_argument("Life is shit");
      }

      _sign = init[0] == '-' ? Sign::Negative : Sign::Positive;
      _value.move_range(begin(init), end(init));
    }

    BigInt(BigInt &&other) noexcept = default;
    BigInt & operator=(BigInt &&other) noexcept = default;

    BigInt(const BigInt &other) noexcept = default;
    BigInt & operator=(const BigInt &other) noexcept = default;

    template <std::integral S> BigInt(S value) { _value.emplace_back(static_cast<T>(value)); }

    bool validate_init_value(mr::StringView<char> init) {
      return true;
    }

    T operator[](std::size_t i) const { return _value[i]; }
    T & operator[](std::size_t i) { return _value[i]; }

    friend bool operator==(const BigInt &lhs, const BigInt &rhs) noexcept {
      return lhs._sign == rhs._sign && lhs._value == rhs._value;
    }
    friend bool operator!=(const BigInt &lhs, const BigInt &rhs) noexcept {
      return !(lhs == rhs);
    }
    friend bool operator<(const BigInt &lhs, const BigInt &rhs) noexcept {
      return lhs._sign == Sign::Negative && rhs._sign == Sign::Positive ||
        (lhs._sign == Sign::Positive ? (lhs._value < rhs._value) : !(lhs._value < rhs._value));
    }
    friend bool operator<=(const BigInt &lhs, const BigInt &rhs) noexcept {
      return lhs < rhs || lhs == rhs;
    }
    friend bool operator>(const BigInt &lhs, const BigInt &rhs) noexcept {
      return !(lhs <= rhs);
    }
    friend bool operator>=(const BigInt &lhs, const BigInt &rhs) noexcept {
      return !(lhs < rhs);
    }

    // addition operators definitions
    friend constexpr BigInt<T> operator+(const BigInt<T> &lhs, const BigInt<T> &rhs) {
      // swap arguments if len(lhs) < len(rhs)
      if (lhs._value.size() < rhs._value.size()) {
        return rhs + lhs;
      }
      // continue assuming len(lhs) >= len(rhs)
      // perform addition
      BigInt tmp = lhs;
      bool trail = false;
      for (std::size_t i = 0; i < rhs._value.size(); i++) {
        bool next_trail = (tmp._value[i] > BigInt<T>::shit_max - std::max<T>(rhs._value[i], trail));
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

    friend constexpr BigInt<T> operator-(const BigInt<T> &lhs, const BigInt<T> &rhs) {
      // swap arguments if len(lhs) < len(rhs)
      if (lhs < rhs) {
        return -(rhs - lhs);
      }

      // continue assuming len(lhs) >= len(rhs)
      // perform addition
      BigInt tmp = lhs;
      bool trail = false;
      for (std::size_t i = 0; i < rhs._value.size(); i++) {
        bool next_trail = (tmp._value[i] < std::max<int>(rhs._value[i], rhs._value[i] + trail));
        tmp._value[i] -= rhs._value[i] + static_cast<T>(trail);
        trail = next_trail;
      }

      return tmp;
    }

    friend constexpr BigInt<T> operator+(const BigInt<T> &rhs) {
      auto tmp = rhs;
      return tmp;
    };

    friend constexpr BigInt<T> operator-(const BigInt<T> &rhs) {
      auto tmp = rhs;
      tmp._sign = BigInt<T>::Sign(!static_cast<int>(tmp._sign));
      return tmp;
    };

    friend constexpr BigInt<T> operator*(const BigInt<T> &lhs, const BigInt<T> &rhs) {
      auto tmp = lhs;
      return tmp;
    }

    friend constexpr BigInt<T> operator*(const BigInt<T> &lhs, T rhs) {
      auto size = lhs.size() + rhs.size();
      BigInt<T> tmp(size, 0);
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
      return {mr::Vector{tmp._value.data(), size}};
    }

    friend constexpr std::tuple<BigInt<T>, T> divide(const BigInt<T> &lhs, T rhs) {
      auto size = lhs._value.size();
      T first = lhs[0];
      BigInt<T> result;
      result._value.resize(size);
      T remainder = 0;
      for (int i = lhs._value.size() - 1; i >= 0; i--) {
        T val = (remainder * (BigInt<T>::shit_max / rhs) + lhs[i] / rhs);
        remainder = remainder * (BigInt<T>::shit_max % rhs) + lhs[i] % rhs;
        result[i] = val;
      }
      if (first == rhs) {
        size--;
      }
      return {mr::Vector{result._value.data(), size}, remainder};
    }

    friend constexpr std::tuple<BigInt<T>, BigInt<T>> divide(const BigInt<T> &lhs, const BigInt<T> &rhs) noexcept {
      BigInt<T> res = 1;
      BigInt<T> remainder = 0;

      return {res, remainder};
    }

    friend constexpr BigInt<T> operator%(const BigInt<T> &lhs, const BigInt<T> &rhs) noexcept {
      auto [_, res] = divide(lhs, rhs);
      return res;
    }
    friend constexpr BigInt<T> operator/(const BigInt<T> &lhs, const BigInt<T> &rhs) noexcept {
      auto [res, _] = divide(lhs, rhs);
      return res;
    }

    friend constexpr T operator%(const BigInt<T> &lhs, T rhs) noexcept {
      auto [_, res] = divide(lhs, rhs);
      return res;
    }
    friend constexpr T operator/(const BigInt<T> &lhs, T rhs) noexcept {
      auto [res, _] = divide(lhs, rhs);
      return res;
    }


    friend constexpr BigInt operator++(BigInt &lhs, int) noexcept { auto tmp = lhs; lhs += 1; return tmp; }
    friend constexpr BigInt operator--(BigInt &lhs, int) noexcept { auto tmp = lhs; lhs -= 1; return tmp; }
    friend constexpr BigInt & operator++(BigInt &rhs) noexcept { return rhs += 1; }
    friend constexpr BigInt & operator--(BigInt &rhs) noexcept { return rhs += 1; }

    friend constexpr BigInt & operator+=(BigInt &lhs, const BigInt &rhs) noexcept { lhs = lhs + rhs; return lhs; }
    friend constexpr BigInt & operator-=(BigInt &lhs, const BigInt &rhs) noexcept { lhs = lhs - rhs; return lhs; }
    friend constexpr BigInt & operator*=(BigInt &lhs, const BigInt &rhs) noexcept { lhs = lhs * rhs; return lhs; }
    friend constexpr BigInt & operator/=(BigInt &lhs, const BigInt &rhs) noexcept { lhs = lhs / rhs; return lhs; }
    friend constexpr BigInt & operator%=(BigInt &lhs, const BigInt &rhs) noexcept { lhs = lhs % rhs; return lhs; }
  };
}
