#ifndef __string_hpp__
#define __string_hpp__

#include "../vector/vector.hpp"

struct String : Vector<char> {
  String() noexcept = default;
  template <typename ...Args>
    String(Args... args) : Vector<char>(args...) {}
  String(const char *str) : Vector<char>(str, strlen(str)) {}

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
};

using StringView = Span<char>;

#endif // __string_hpp__
