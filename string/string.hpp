#ifndef __string_hpp__
#define __string_hpp__

#include "../vector/vector.hpp"

struct String : Vector<char> {
  String() noexcept = default;
  template <typename ...Args>
    String(Args... args) : Vector<char>(args...) {}
  String(const char *str) : Vector<char>(str, strlen(str)) {}

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
    String tmp {_size + other._size};

    std::memcpy(tmp._data.data(), _data.data(), _size);
    std::memcpy(tmp._data.data() + _size, other._data.data(), other._size);

    return tmp;
  }
};

using StringView = Span<char>;

#endif // __string_hpp__
