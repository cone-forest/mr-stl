#ifndef __def_hpp__
#define __def_hpp__

#include <functional>
#if __cpp_lib_span >= 202002L
#include <span>
#endif
#include <cstddef>
#include <cstring>
#include <optional>
#include <iostream>
#include <cstdint>
#include <utility>
#include <memory>

namespace mr {
  template <typename T>
    concept Range = requires (T a) {
      begin(a);
      end(a);
    };

  template <typename T>
    concept LimitedRange = requires {
      requires Range<T>;
      T::size();
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

  void reverse(LimitedRange auto &range) {
    for (std::size_t i = 0; i < range.size() / 2; i++) {
      std::swap(range[i], range[range.size() - i - 1]);
    }
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
