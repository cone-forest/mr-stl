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
    BigInt(mr::StringView<char> init) : BigInt(0) {
      if (!validate_init_value(init)) {
        throw std::invalid_argument("Invalid argument");
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

    BigInt(std::integral auto init) : _sign(init < 0 ? Sign::Negative : Sign::Positive) {
      constexpr std::size_t size = sizeof(init) / sizeof(T);
      constexpr T mask = std::numeric_limits<T>::max();
      for (int i = 0; i < size; i++) {
      }

      _value.emplace_back(static_cast<T>(init < 0 ? -init : init));
    }

    bool validate_init_value(mr::StringView<char> init) {
      bool digit_found = false;
      for (int i = 0; i < init.size(); i++) {
        if (!std::isdigit(init[i])) {
          if (i == 0 && (init[i] == '-' || init[i] == '+')) continue;
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

    friend bool operator==(const BigInt &lhs, std::integral auto rhs) {
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

    friend BigInt operator<<(const BigInt<T> &lhs, std::size_t rhs) {
      assert(rhs < sizeof(T) * 8);

      T carry = 0;
      BigInt<T> copy = lhs;
      for (int i = 0; i < lhs.size(); i++) {
        copy[i] = (lhs[i] << rhs) | carry;
        carry = (lhs[i] >> (sizeof(T) * 8 - rhs)) & ((1 << rhs) - 1);
      }

      if (carry != 0) {
        copy._value.emplace_back(carry);
      }

      return std::move(copy);
    }

    friend BigInt operator>>(const BigInt<T> &lhs, std::size_t rhs) {
      assert(rhs < sizeof(T) * 8);

      T carry = 0;
      BigInt<T> copy = lhs;
      for (int i = lhs.size() - 1; i >= 0; i--) {
        copy[i] = (lhs[i] >> rhs) + carry;
        carry = (lhs[i] & ((1 << rhs) - 1)) << (sizeof(T) * 8 - rhs);
      }

      return std::move(copy);
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

    friend constexpr BigInt<T> operator*(const BigInt<T> &lhs, T rhs) {
      constexpr std::size_t halfbitsize = sizeof(T) * 4;
      constexpr std::size_t lomask = (1 << halfbitsize) - 1;
      constexpr std::size_t himask = lomask << halfbitsize;

      size_t size = lhs.size();
      BigInt<T> tmp;
      T carry = 0;
      for (int i = 0; i < lhs.size(); i++) {
        T a0 = lhs[i] & himask;
        T a1 = lhs[i] & lomask;

        T b0 = rhs & himask;
        T b1 = rhs & lomask;

        T res = a0 * b1 + a1 * b1 + a1 * b0;
        T rem = (a0 >> halfbitsize) * (b0 >> halfbitsize);

        tmp._value.emplace_back(res + carry);
        carry = rem;
      }

      return std::move(tmp);
    }

    friend constexpr std::tuple<BigInt<T>, T> divmod(const BigInt<T> &lhs, T rhs) {
      auto [res, rem] = divmod(lhs, BigInt(rhs));
      return {std::move(res), rem[0]};
    }

    friend constexpr std::tuple<BigInt<T>, BigInt<T>> divmod(const BigInt<T> &lhs, const BigInt<T> &rhs) noexcept {
      std::pair<BigInt<T>, BigInt<T>> qr;
      if (is_neutral(lhs)) {
        qr.first = 0;
        qr.second = 0;
        return qr;
      }

      qr = divmod(lhs >> 1, rhs);
      qr.first <<= 1;
      qr.second <<= 1;
      if (lhs[0] & 1) {
        qr.second++;
      }
      if (qr.second >= rhs) {
        qr.second -= rhs;
        qr.first++;
      }
      return qr;
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
    friend constexpr BigInt & operator<<=(BigInt &lhs, T rhs) noexcept { lhs = lhs << rhs; return lhs; }
    friend constexpr BigInt & operator>>=(BigInt &lhs, T rhs) noexcept { lhs = lhs >> rhs; return lhs; }
    friend constexpr BigInt & operator*=(BigInt &lhs, T rhs) noexcept { lhs = lhs * rhs; return lhs; }
    friend constexpr BigInt & operator/=(BigInt &lhs, T rhs) noexcept { lhs = lhs / rhs; return lhs; }
    friend constexpr BigInt & operator%=(BigInt &lhs, T rhs) noexcept { lhs = lhs % rhs; return lhs; }

    friend BigInt::Sign negate(BigInt::Sign sign) { return sign == Sign::Positive ? Sign::Negative : Sign::Positive; }

    friend std::ostream & operator<<(std::ostream &out, const BigInt<T> &value) noexcept {
      BigInt<T> copy = value;
      for (int i = copy.size(); i >= 0; i--) {
        out << copy[i] << ' ';
      }
      return out;

      while (copy > 0) {
        out << ('0' + copy % 10);
        copy /= 10;
      }
      return out;
    }
  };
}
