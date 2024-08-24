#pragma once

#include <string_view>
#include "mr-stl/vector/vector.hpp"

namespace mr {
  template <typename C = char>
    struct String : Vector<C>,
                    FlatRangeMethods<String, C>,
                    RangeOutputOperators<String, C> {
      String() noexcept = default;
      template <typename ...Args>
        String(Args... args) : Vector<C>(std::forward<Args>(args)...) {}
      String(const C *str) : Vector<C>(str, strlen(str)) {}
      String(C *str) : Vector<C>((const C *)str, strlen(str)) {}

      // move semantic
      String(String &&other) noexcept = default;
      String & operator=(String &&other) noexcept = default;

      ~String() noexcept = default;

      struct Hash {
        std::size_t operator()(const String &s){
          // pick 5 characters and sum them up
          char range[5] {};
          for (int i = 0; i < 5; i++) {
            if (i > s.size()) {
            }
          }
          return 0;
        }
      };

      String operator+(const String &other) const noexcept {
        String tmp {Vector<C>::_size + other._size};

        std::memcpy(tmp._data.data(), Vector<C>::_data.data(), Vector<C>::_size);
        std::memcpy(tmp._data.data() + Vector<C>::_size, other._data.data(), other._size);

        return tmp;
      }
    };

  template <typename C = char>
    struct StringView : std::string_view<C> {
      using string_view<C>::string_view;
      StringView(const String<C> &str) :
        string_view<C>(str.data(), str.size()) {}
    };
}
