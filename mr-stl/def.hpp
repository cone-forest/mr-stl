#ifndef __def_hpp__
#define __def_hpp__

#include <functional>
#include <span>
#include <cstddef>
#include <cstring>
#include <optional>
#include <iostream>
#include <algorithm>
#include <cstdint>
#include <memory>

namespace mr {
  template <typename T>
    concept Range = requires {
      T::begin();
      T::end();
    };

  template <Range R, typename Compare>
    bool contains(R range, Compare cmp) {
      for (auto elem : range) {
        if (cmp(elem)) {
          return true;
        }
      }
      return false;
    }

  template<class... Ts>
    struct overloads : Ts... { using Ts::operator()...; };

  template <typename ...Args>
    bool all(Args ...args) { return (args && ...); }

  template <typename T, typename ...Args, typename F>
    requires std::is_invocable_r_v<T, F, Args...>
  std::optional<T> and_then(std::optional<Args> ...optionals, F &&f) {
    if (all(optionals.has_value()...)) { return f(optionals.value()...); }
    return std::nullopt;
  }

  template <typename T, typename ...Args, typename F>
    requires std::is_invocable_r_v<std::optional<T>, F, Args...>
  std::optional<T> and_then(std::optional<Args> ...optionals, F &&f) {
    if (all(optionals.has_value()...)) {
      return f(optionals.value()...);
    }
    return std::nullopt;
  }

  template <typename S, typename R, typename ...Args>
    auto bind_self(R (S::* f)(Args...), S* self) { return std::bind_front(f, self); }

  template <template <typename> typename Range, typename T>
    struct FlatRangeMethods {
      using RangeT = Range<T>;

      friend T * begin(RangeT &self) noexcept {
        return self.data();
      }

      friend T * end(RangeT &self) noexcept {
        return self.data() + self.size();
      }

      friend const T * begin(const RangeT &self) noexcept {
        return self.data();
      }

      friend const T * end(const RangeT &self) noexcept {
        return self.data() + self.size();
      }

      friend const T * cbegin(const RangeT &self) noexcept {
        return self.data();
      }

      friend const T * cend(const RangeT &self) noexcept {
        return self.data() + self.size();
      }
    };

  template <template <typename> typename Range, typename T>
    struct RangeOutputOperators {
      using RangeT = Range<T>;

      friend std::ostream & operator<<(std::ostream &out, const RangeT &range) {
        for (const T &elem : range) {
          out << elem << ' ';
        }
        out << '\n';
        return out;
      }
    };
}

#endif // __def_hpp__
