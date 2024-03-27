#include <limits>
#include "mr-stl/vector/vector.hpp"

namespace mr {
  // forward declaration
  struct BigInt;
  template <typename T> struct PositiveRef;
  template <typename T> struct NegativeRef;

  template <typename T>
    struct PositiveRef {
      const T &ref;
      PositiveRef(const T &r) : ref(r) {}
      operator const T &() { return ref; }
      friend NegativeRef<T> operator-(PositiveRef<T> self) {
        return self.ref;
      }
    };

  template <typename T>
    struct NegativeRef {
      const T &ref;
      NegativeRef(const T &r) : ref(r) {}
      operator const T &() { return ref; }
      friend PositiveRef<T> operator-(NegativeRef<T> self) {
        return self.ref;
      }
    };

  template <typename T>
    struct SignedRef {
      const T &ref;
      bool is_negative = false;

      SignedRef<T>(const T &r, bool is_neg) : ref(r), is_negative(is_neg) {}

      SignedRef<T> operator-() const {
        return SignedRef<T>(*this, !is_negative);
      }
    };

  struct BigInt {
    inline static constexpr std::uint32_t shit_max = std::numeric_limits<std::uint32_t>::max();
    using PositiveRefT = PositiveRef<BigInt>;
    using NegativeRefT = NegativeRef<BigInt>;
    using SignedRefT = SignedRef<BigInt>;

    // absolute
    Vector<std::uint32_t> _value;

    BigInt() noexcept = default;

    BigInt(BigInt &&other) noexcept = default;
    BigInt & operator=(BigInt &&other) noexcept = default;

    BigInt(const BigInt &other) noexcept = default;
    BigInt & operator=(const BigInt &other) noexcept = default;

    template <std::integral T> BigInt(T value) { _value.emplace_back(value); }

    // addition operators
    friend PositiveRefT operator+(PositiveRefT lhs, PositiveRefT rhs);
    friend SignedRefT   operator+(PositiveRefT lhs, NegativeRefT rhs);
    friend SignedRefT   operator+(NegativeRefT lhs, PositiveRefT rhs);
    friend NegativeRefT operator+(NegativeRefT lhs, NegativeRefT rhs);

    // subtraction operators
    friend SignedRefT   operator-(PositiveRefT lhs, PositiveRefT rhs);
    friend PositiveRefT operator-(PositiveRefT lhs, NegativeRefT rhs);
    friend NegativeRefT operator-(NegativeRefT lhs, PositiveRefT rhs);
    friend SignedRefT   operator-(NegativeRefT lhs, NegativeRefT rhs);

    // multiplication operators
    friend PositiveRefT operator*(PositiveRefT lhs, PositiveRefT rhs);
    friend NegativeRefT operator*(PositiveRefT lhs, NegativeRefT rhs);
    friend NegativeRefT operator*(NegativeRefT lhs, PositiveRefT rhs);
    friend PositiveRefT operator*(NegativeRefT lhs, NegativeRefT rhs);

    // division operators
    friend PositiveRefT operator/(PositiveRefT lhs, PositiveRefT rhs);
    friend NegativeRefT operator/(PositiveRefT lhs, NegativeRefT rhs);
    friend NegativeRefT operator/(NegativeRefT lhs, PositiveRefT rhs);
    friend PositiveRefT operator/(NegativeRefT lhs, NegativeRefT rhs);

    BigInt operator-(PositiveRefT other) {
      BigInt tmp = *this;
      bool trail = false;

      for (std::size_t i = 0; i < _value.size(); i++) {
        bool next_trail = (tmp._value[i] > shit_max - other.ref._value[i]);
        tmp._value[i] += other.ref._value[i] + static_cast<std::uint32_t>(trail);
        trail = next_trail;
      }

      _value.emplace_back(static_cast<std::uint32_t>(trail));
      return tmp;
    }

    std::uint32_t operator[](std::size_t i) const { return _value[i]; }
    std::uint32_t & operator[](std::size_t i) { return _value[i]; }
  };

  // addition operators definitions
  BigInt::PositiveRefT operator+(BigInt::PositiveRefT lhs, BigInt::PositiveRefT rhs) {
    BigInt tmp = lhs;
    bool trail = false;

    for (std::size_t i = 0; i < lhs.ref._value.size(); i++) {
      bool next_trail = (tmp._value[i] > BigInt::shit_max - rhs.ref._value[i]);
      tmp._value[i] += rhs.ref._value[i] + static_cast<std::uint32_t>(trail);
      trail = next_trail;
    }

    tmp._value.emplace_back(static_cast<std::uint32_t>(trail));
    return BigInt::PositiveRefT(tmp);
  }
  BigInt::SignedRefT   operator+(BigInt::PositiveRefT lhs, BigInt::NegativeRefT rhs) {
    return lhs - BigInt::PositiveRefT(rhs);
  }
  BigInt::SignedRefT   operator+(BigInt::NegativeRefT lhs, BigInt::PositiveRefT rhs) {
    return -(rhs + lhs);
  }
  BigInt::NegativeRefT operator+(BigInt::NegativeRefT lhs, BigInt::NegativeRefT rhs) {
    return -(BigInt::PositiveRefT(lhs) + BigInt::PositiveRefT(rhs));
  }

  // subtraction operators
  BigInt::SignedRefT   operator-(BigInt::PositiveRefT lhs, BigInt::PositiveRefT rhs);
  BigInt::PositiveRefT operator-(BigInt::PositiveRefT lhs, BigInt::NegativeRefT rhs);
  BigInt::NegativeRefT operator-(BigInt::NegativeRefT lhs, BigInt::PositiveRefT rhs);
  BigInt::SignedRefT   operator-(BigInt::NegativeRefT lhs, BigInt::NegativeRefT rhs);

  // multiplication operators
  BigInt::PositiveRefT operator*(BigInt::PositiveRefT lhs, BigInt::PositiveRefT rhs);
  BigInt::NegativeRefT operator*(BigInt::PositiveRefT lhs, BigInt::NegativeRefT rhs);
  BigInt::NegativeRefT operator*(BigInt::NegativeRefT lhs, BigInt::PositiveRefT rhs);
  BigInt::PositiveRefT operator*(BigInt::NegativeRefT lhs, BigInt::NegativeRefT rhs);

  // division operators
  BigInt::PositiveRefT operator/(BigInt::PositiveRefT lhs, BigInt::PositiveRefT rhs);
  BigInt::NegativeRefT operator/(BigInt::PositiveRefT lhs, BigInt::NegativeRefT rhs);
  BigInt::NegativeRefT operator/(BigInt::NegativeRefT lhs, BigInt::PositiveRefT rhs);
  BigInt::PositiveRefT operator/(BigInt::NegativeRefT lhs, BigInt::NegativeRefT rhs);
}
