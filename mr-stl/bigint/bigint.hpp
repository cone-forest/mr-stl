#include <cassert>
#include <cstdint>
#include <limits>
#include <cmath>

#include "mr-stl/string/string.hpp"
#include "mr-stl/vector/vector.hpp"

namespace mr {
  template <std::integral T>
  std::tuple<T, T> multiply(T a, T b) {
    constexpr std::uint64_t halfbitsize = sizeof(T) * 4;
    constexpr std::uint64_t lomask = (1ull << halfbitsize) - 1;
    constexpr std::uint64_t himask = lomask << halfbitsize;
    constexpr auto lo = [=](T e) -> T { return e & lomask; };
    constexpr auto hi = [=](T e) -> T { return (e >> halfbitsize) & lomask; };

    T s0, s1, s2, s3;

    T x = lo(a) * lo(b);
    s0 = lo(x);

    x = hi(a) * lo(b) + hi(x);
    s1 = lo(x);
    s2 = hi(x);

    x = s1 + lo(a) * hi(b);
    s1 = lo(x);

    x = s2 + hi(a) * hi(b) + hi(x);
    s2 = lo(x);
    s3 = hi(x);

    T res = s1 << halfbitsize | s0;
    T rem = s3 << halfbitsize | s2;
    return {res, rem};
    }

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

      _value.emplace_back(0u);

      for (char e : init) {
        if (std::isdigit(e)) [[likely]] {
          *this *= 10;
          *this += e - '0';
        }
      }
      _sign = init[0] == '-' ? Sign::Negative : Sign::Positive;
    }

    BigInt(BigInt &&other) noexcept = default;
    BigInt & operator=(BigInt &&other) noexcept = default;

    BigInt(const BigInt &other) noexcept = default;
    BigInt & operator=(const BigInt &other) noexcept = default;

    BigInt(std::integral auto init) : _sign(init < 0 ? Sign::Negative : Sign::Positive) {
      if (init == 0) {
        return;
      }
      constexpr std::size_t size = std::max<std::size_t>(sizeof(init) / sizeof(T), 1);
      constexpr auto mask = (std::make_unsigned_t<decltype(init)>)std::numeric_limits<T>::max();
      init = init < 0 ? -init : init;
      for (int i = 0; i < size; i++) {
        int shift = (sizeof(T) * 8 * i);
        T value = (init & (mask << shift)) >> shift;
        _value.emplace_back(value);
      }
      trim();
    }

    BigInt & trim() {
      auto size = _value.size();
      while (size > 0 && _value[size-1] == 0) {
        size--;
      }
      _value.size(size);
      return *this;
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
      return other.size() == 0 || other.size() == 1 && other[0] == 0;
    }

    friend BigInt<T> abs(const BigInt<T>& other) noexcept {
      BigInt<T> res = other;
      res._sign = Sign::Positive;
      return std::move(res);
    }

    // getters
    T operator[](std::size_t i) const { return _value[i]; }
    T & operator[](std::size_t i) {
      if (i >= _value.size()) {
        std::cout << "life is shit\n";
      }
      return _value[i];
    }
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
      if (is_neutral(lhs)) {
        if (is_neutral(rhs)) {
          return false;
        } else {
          return rhs._sign == Sign::Positive;
        }
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
      return false;
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
      T carry = 0;
      BigInt<T> copy = lhs;
      while (rhs >= sizeof(T) * 8) {
        copy._value.emplace_at((size_t)0, (T)0);
        rhs -= sizeof(T) * 8;
      }
      if (rhs == 0) {
        return std::move(copy);
      }
      for (int i = 0; i < lhs.size(); i++) {
        copy[i] = (lhs[i] << rhs) | carry;
        carry = (lhs[i] >> (sizeof(T) * 8 - rhs)) & ((1ull << rhs) - 1);
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
        carry = (lhs[i] & ((1ull << rhs) - 1)) << (sizeof(T) * 8 - rhs);
      }

      copy.trim();
      return std::move(copy);
    }

    // addition operators definitions
    friend constexpr BigInt<T> operator+(const BigInt<T> &lhs, const BigInt<T> &rhs) {
      // swap arguments if len(lhs) < len(rhs)
      if (is_neutral(lhs)) {
        return +rhs;
      }
      if (is_neutral(rhs)) {
        return +lhs;
      }
      if (lhs.size() < rhs.size()) {
        return rhs + lhs;
      }
      if (lhs._sign != rhs._sign) {
        if (lhs._sign == Sign::Negative) {
          return rhs - -lhs;
        }
        return lhs - -rhs;
      }

      // continue assuming len(lhs) >= len(rhs)
      // perform addition
      BigInt tmp = lhs;
      bool trail = false;
      for (std::size_t i = 0; i < rhs.size(); i++) {
        bool next_trail = (tmp[i] > BigInt<T>::shit_max - std::max<T>(rhs[i], trail));
        tmp[i] += rhs[i] + static_cast<T>(trail);
        trail = next_trail;
      }

      int i = rhs.size();
      while (trail && i < lhs.size()) {
        trail = ++tmp[i++] == 0;
      }
      if (trail) {
        tmp._value.emplace_back((T)trail);
      }

      return tmp;
    }

    friend constexpr BigInt<T> operator-(const BigInt<T> &lhs, const BigInt<T> &rhs) {
      // swap arguments if len(lhs) < len(rhs)
      if (is_neutral(lhs)) {
        return -rhs;
      }
      if (is_neutral(rhs)) {
        return +lhs;
      }
      if (lhs._sign != rhs._sign) {
        return lhs + -rhs;
      }
      if (lhs < rhs) {
        return -(rhs - lhs);
      }
      if (rhs._sign == Sign::Negative) {
        return lhs + -rhs;
      }

      BigInt out = lhs;
      for(int x = 0; x < rhs.size(); x++){
        if (out[x] >= rhs[x]){
          out[x] -= rhs[x];
        } else {
          std::size_t y = x + 1;
          while (out[y] == 0){
            y++;
          }

          out[y]--;

          for(y--; y > x; y--){
            out[y] = std::numeric_limits<T>::max();
          }

          out[x] += std::numeric_limits<T>::max() - rhs[x] + 1;
        }
      }

      return std::move(out.trim());
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

      BigInt<T> res;
      for (int i = 0; i < rhs.size(); i++) {
        res += (lhs * rhs[i]) << (sizeof(T) * 8 * i);
      }
      res._sign = lhs._sign == rhs._sign ? Sign::Positive : Sign::Negative;
      return std::move(res);
    }

    friend constexpr BigInt<T> operator*(const BigInt<T> &lhs, T rhs) {
      size_t size = lhs.size();
      BigInt<T> tmp;
      T carry = 0;
      for (int i = 0; i < lhs.size(); i++) {
        auto [res, rem] = multiply(lhs[i], rhs);
        tmp._value.emplace_back(res + carry);
        carry = rem + (res > std::numeric_limits<T>::max() - carry);
      }

      if (carry != 0) {
        tmp._value.emplace_back(carry);
      }

      return std::move(tmp.trim());
    }

    friend constexpr std::tuple<BigInt<T>, T> divmod(const BigInt<T> &lhs, T rhs) {
      auto [res, rem] = divmod(lhs, BigInt(rhs));
      return {is_neutral(res) ? 0 : std::move(res),
              is_neutral(rem) ? 0 : rem[0] };
    }

    friend constexpr std::tuple<BigInt<T>, BigInt<T>> divmod(const BigInt<T> &lhs, const BigInt<T> &rhs) noexcept {
      // assume lhs >= 0 && rhs >= 0
      if (is_neutral(lhs)) {
        return {{}, {}};
      }
      if (lhs < rhs) {
        return {{}, {lhs}};
      }
      if (lhs == rhs) {
        return {1, 0};
      }
      /*
      if (lhs._sign == Sign::Negative) {
        return divmod(abs(lhs), rhs);
      }
      if (rhs._sign == Sign::Negative) {
        return divmod(lhs, abs(rhs));
      }
      */

      auto [res, rem] = divmod(lhs >> 1, rhs);
      res <<= 1;
      rem <<= 1;
      if (lhs[0] & 1) {
        rem++;
      }
      if (rem >= rhs) {
        rem -= rhs;
        res++;
      }
      res.trim();
      rem.trim();
      /*
      res._sign = (lhs._sign == rhs._sign ? Sign::Positive : Sign::Negative);
      rem._sign = lhs._sign;
      */
      return {std::move(res), std::move(rem)};
    }

    friend constexpr T operator%(const BigInt<T> &lhs, std::integral auto rhs) noexcept {
      using std::abs;
      auto [_, res] = divmod(abs(lhs), abs(rhs));
      res._sign = lhs._sign;
      return std::move(res);
    }
    friend constexpr BigInt<T> operator/(const BigInt<T> &lhs, std::integral auto rhs) noexcept {
      using std::abs;
      auto [res, _] = divmod(abs(lhs), abs(rhs));
      res._sign = lhs._sign == Sign(rhs >= 0);
      return std::move(res);
    }

    friend constexpr BigInt<T> operator%(const BigInt<T> &lhs, const BigInt<T> &rhs) noexcept {
      using std::abs;
      auto [_, res] = divmod(abs(lhs), abs(rhs));
      res._sign = lhs._sign;
      return std::move(res);
    }
    friend constexpr BigInt<T> operator/(const BigInt<T> &lhs, const BigInt<T> &rhs) noexcept {
      auto [res, _] = divmod(abs(lhs), abs(rhs));
      res._sign = lhs._sign == rhs._sign ? Sign::Positive : Sign::Negative;
      return std::move(res);
    }

    friend constexpr BigInt operator++(BigInt &lhs, int) noexcept { auto tmp = lhs; lhs += 1; return tmp; }
    friend constexpr BigInt operator--(BigInt &lhs, int) noexcept { auto tmp = lhs; lhs -= 1; return tmp; }
    friend constexpr BigInt & operator++(BigInt &rhs) noexcept { return rhs += 1; }
    friend constexpr BigInt & operator--(BigInt &rhs) noexcept { return rhs -= 1; }

    friend constexpr BigInt & operator+=(BigInt &lhs, const BigInt &rhs) noexcept { lhs = lhs + rhs; return lhs; }
    friend constexpr BigInt & operator-=(BigInt &lhs, const BigInt &rhs) noexcept { lhs = lhs - rhs; return lhs; }
    friend constexpr BigInt & operator*=(BigInt &lhs, const BigInt &rhs) noexcept { lhs = lhs * rhs; return lhs; }
    friend constexpr BigInt & operator/=(BigInt &lhs, const BigInt &rhs) noexcept { lhs = lhs / rhs; return lhs; }
    friend constexpr BigInt & operator%=(BigInt &lhs, const BigInt &rhs) noexcept { lhs = lhs % rhs; return lhs; }

    friend constexpr BigInt & operator+=(BigInt &lhs,  std::integral auto rhs) noexcept { lhs = lhs + rhs; return lhs; }
    friend constexpr BigInt & operator-=(BigInt &lhs,  std::integral auto rhs) noexcept { lhs = lhs - rhs; return lhs; }
    friend constexpr BigInt & operator<<=(BigInt &lhs, std::integral auto rhs) noexcept { lhs = lhs << rhs; return lhs; }
    friend constexpr BigInt & operator>>=(BigInt &lhs, std::integral auto rhs) noexcept { lhs = lhs >> rhs; return lhs; }
    friend constexpr BigInt & operator*=(BigInt &lhs,  std::integral auto rhs) noexcept { lhs = lhs * rhs; return lhs; }
    friend constexpr BigInt & operator/=(BigInt &lhs,  std::integral auto rhs) noexcept { lhs = lhs / rhs; return lhs; }
    friend constexpr BigInt & operator%=(BigInt &lhs,  std::integral auto rhs) noexcept { lhs = lhs % rhs; return lhs; }

    friend BigInt::Sign negate(BigInt::Sign sign) { return sign == Sign::Positive ? Sign::Negative : Sign::Positive; }
    friend void negate(BigInt &other) {
      other._sign = negate(other._sign);
    }

    friend void print(std::ostream &out, BigInt<T> &value) {
      auto [res, rem] = divmod(value, 10);
      value = std::move(res);
      if (!is_neutral(value)) {
        print(out, value);
      }
      out << (char)('0' + rem);
    }

    friend std::ostream & operator<<(std::ostream &out, const BigInt<T> &value) noexcept {
      BigInt<T> copy = value;
      if (!is_neutral(copy) && copy._sign == Sign::Negative) {
        out << '-';
        negate(copy);
      }
      print(out, copy);
      return out;
    }
  };
}
