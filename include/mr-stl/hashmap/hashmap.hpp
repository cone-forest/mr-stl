#pragma once

#include "mr-stl/def.hpp"

namespace mr {
  template <typename K, typename V, typename H = std::hash<K>>
    struct StaticHashmap {
      inline static constexpr std::size_t length = 1024;
      inline static constexpr auto hash = [](){
        if constexpr (std::is_same_v<H, std::hash<K>> &&
            !std::is_invocable_v<H, K>) {
          return typename K::Hash{};
        } else {
          return H{};
        }
      }();
      std::size_t _size = 0;

      // array of optional values
      std::optional<std::pair<K, V>> data[length] {};

      std::optional<StaticHashmap> emplace(const K &key, const V &value) {
        if (_size == length) {
          return *this;
        }

        std::size_t hash_res = hash(key) & (length - 1);
        hash_res = handle_collision(hash_res, key);
        data[hash_res] = {key, value};
        _size++;

        return *this;
      }

      std::optional<V> find(const K &key) {
        std::size_t hash_res = hash(key) & (length - 1);
        hash_res = handle_collision(hash_res, key);
        return data[hash_res].and_then([](auto pair){
            return std::optional(pair.second);
            });
      }

      std::size_t handle_collision(std::size_t ind, K key) {
        while (data[ind].has_value() &&
            data[ind]->first != key) [[unlikely]] {
          ind++;
          ind %= length;
        }
        return ind;
      }

      StaticHashmap & get() {
        return *this;
      }
    };
}
