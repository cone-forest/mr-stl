#ifndef __def_hpp__
#define __def_hpp__

#include <span>
#include <cstddef>
#include <cstring>
#include <optional>
#include <iostream>
#include <memory>

namespace mr {
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
