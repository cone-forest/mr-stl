#include <cassert>
#include <cstdint>
#include <limits>
#include <cmath>

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
    BigInt(mr::StringView<char> init) : BigInt(0) {
      if (!validate_init_value(init)) {
        throw std::invalid_argument("Life is shit");
      }

      _sign = init[0] == '-' ? Sign::Negative : Sign::Positive;
      for (char e : init) {
        if (std::isdigit(e)) [[likely]] {
          *this *= 10;
          *this += e - '0';
        }
      }
    }

    BigInt(BigInt &&other) noexcept = default;
    BigInt & operator=(BigInt &&other) noexcept = default;

    BigInt(const BigInt &other) noexcept = default;
    BigInt & operator=(const BigInt &other) noexcept = default;

    template <std::integral S> BigInt(S value) { _value.emplace_back(static_cast<T>(value)); }

    bool validate_init_value(mr::StringView<char> init) {
      bool digit_found = false;
      for (int i = 0; i < init.size(); i++) {
        if (!std::isdigit(init[i])) {
          if (i == 0 && init[i] == '-') continue;
          return false;
        } else {
          digit_found = true;
        }
      }

      return digit_found;
    }

    friend bool is_neutral(const BigInt &other) noexcept {
      return other.size() == 0 || other.size() == 1 && other._value[0] == 0;
    }

    // getters
    T operator[](std::size_t i) const { return _value[i]; }
    T & operator[](std::size_t i) { return _value[i]; }
    std::size_t size() const { return _value.size(); }

    template <std::integral S>
      friend bool operator==(const BigInt &lhs, S rhs) {
        return lhs == BigInt(rhs);
      }
    friend bool operator==(const BigInt &lhs, const BigInt &rhs) noexcept {
      return is_neutral(lhs) && is_neutral(rhs) ||
        lhs._sign == rhs._sign && lhs._value == rhs._value;
    }
    friend bool operator!=(const BigInt &lhs, const BigInt &rhs) noexcept {
      return !(lhs == rhs);
    }
    friend bool operator<(const BigInt &lhs, const BigInt &rhs) noexcept {
      if (is_neutral(lhs) && is_neutral(rhs)) {
        return false;
      }
      if (lhs._sign != rhs._sign) {
        return lhs._sign == Sign::Negative && rhs._sign == Sign::Positive;
      }

      bool negative = lhs._sign == Sign::Negative;
      bool res = false;
      if (lhs.size() != rhs.size()) {
         res = (lhs.size() < rhs.size());
         return negative ? !res : res;
      }

      for (int i = lhs.size() - 1; i >= 0; i--) {
        if (lhs[i] != rhs[i]) {
          res = lhs[i] < rhs[i];
          return negative ? !res : res;
        }
      }
      return negative ? !res : res;
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
      if (lhs.size() < rhs.size()) {
        return rhs + lhs;
      }
      // continue assuming len(lhs) >= len(rhs)
      // perform addition
      BigInt tmp = lhs;
      bool trail = false;
      for (std::size_t i = 0; i < rhs.size(); i++) {
        bool next_trail = (tmp._value[i] > BigInt<T>::shit_max - std::max<T>(rhs._value[i], trail));
        tmp._value[i] += rhs._value[i] + static_cast<T>(trail);
        trail = next_trail;
      }

      // handle trailing 1
      if (trail) {
        if (lhs.size() != rhs.size()) {
          tmp[rhs.size()] += trail;
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
      for (std::size_t i = 0; i < rhs.size(); i++) {
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
      tmp._sign = negate(tmp._sign);
      return tmp;
    };

    friend BigInt<T> peasant(const BigInt<T> & lhs, const BigInt<T> & rhs) {
      BigInt<T> rhs_copy = rhs;
      BigInt<T> sum = 0;

      for (std::size_t x = 0; x < lhs.size(); x++) {
        if (lhs[x]) {
          sum += sum + rhs_copy;
        }
        rhs_copy *= 2;
      }

      return sum;
    }

    friend BigInt<T> karatsuba(const BigInt<T> & lhs, const BigInt<T> & rhs, T bm = 0x100000) {
      if ((lhs <= bm) || (rhs <= bm)) {
        return peasant(lhs, rhs);
      }

      auto [x0, x1] = divmod(lhs, bm);
      auto [y0, y1] = divmod(lhs, bm);

      BigInt<T> z0 = karatsuba(x0, y0);
      BigInt<T> z2 = karatsuba(BigInt<T>(x1), BigInt<T>(y1));
      BigInt<T> z1 = karatsuba(x1 + x0, y1 - y0) - z2 - z0;
      return peasant(peasant(z2, bm) + z1, bm) + z0;
    }

    friend constexpr BigInt<T> operator*(const BigInt<T> &lhs, const BigInt<T> &rhs) {
      if (is_neutral(lhs) || is_neutral(rhs)) {
        return 0;
      }
      if (lhs == 1) {
        return rhs;
      }
      if (rhs == 1) {
        return lhs;
      }

      BigInt<T> tmp = karatsuba(lhs, rhs);
      tmp._sign = lhs._sign == rhs._sign ? Sign::Positive : Sign::Negative;
      return std::move(tmp);
    }

    friend constexpr std::tuple<BigInt<T>, T> divmod(const BigInt<T> &lhs, T rhs) {
      auto [res, rem] = divmod(lhs, BigInt(rhs));
      return {std::move(res), rem[0]};
    }

    friend constexpr std::tuple<BigInt<T>, BigInt<T>> divmod(const BigInt<T> &lhs, const BigInt<T> &rhs) noexcept {
      // naive implementation
      BigInt<T> res = 0;
      BigInt<T> rem = lhs;

      while (rem >= rhs) {
        rem -= lhs;
        res++;
      }

      return {res, rem};
    }

    friend constexpr T operator%(const BigInt<T> &lhs, T rhs) noexcept {
      auto [_, res] = divmod(lhs, rhs);
      return res;
    }
    friend constexpr BigInt<T> operator/(const BigInt<T> &lhs, T rhs) noexcept {
      auto [res, _] = divmod(lhs, rhs);
      return res;
    }

    friend constexpr BigInt<T> operator%(const BigInt<T> &lhs, const BigInt<T> &rhs) noexcept {
      auto [_, res] = divmod(lhs, rhs);
      return res;
    }
    friend constexpr BigInt<T> operator/(const BigInt<T> &lhs, const BigInt<T> &rhs) noexcept {
      auto [res, _] = divmod(lhs, rhs);
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

    friend constexpr BigInt & operator+=(BigInt &lhs, T rhs) noexcept { lhs = lhs + rhs; return lhs; }
    friend constexpr BigInt & operator-=(BigInt &lhs, T rhs) noexcept { lhs = lhs - rhs; return lhs; }
    friend constexpr BigInt & operator*=(BigInt &lhs, T rhs) noexcept { lhs = lhs * rhs; return lhs; }
    friend constexpr BigInt & operator/=(BigInt &lhs, T rhs) noexcept { lhs = lhs / rhs; return lhs; }
    friend constexpr BigInt & operator%=(BigInt &lhs, T rhs) noexcept { lhs = lhs % rhs; return lhs; }

    friend BigInt::Sign negate(BigInt::Sign sign) { return sign == Sign::Positive ? Sign::Negative : Sign::Positive; }
  };
}
