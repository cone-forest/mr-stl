#pragma once

#include <functional>
#include "mr-stl/vector/vector.hpp"
// #include "mr-stl/vector/amortized_vector.hpp"

namespace mr {
  template <typename T>
    class Graph {
    public:
      using Node = T;
      using Destination = std::size_t;
      using Edge = std::pair<Destination, Destination>;
      using Path = mr::Vector<Node>;

    private:
      mr::Vector<Destination> _destinations;
      mr::Vector<Destination> _destinations_lookup;
      mr::Vector<Node> _nodes;

    public:
      std::optional<std::size_t> find(const T &node) const {
        for (std::size_t i = 0; i < _nodes.size(); ++i) {
          if (_nodes[i] == node) {
            return i;
          }
        }
        return std::nullopt;
      }

      template <typename Fn> requires (std::is_invocable_v<Fn, Node>)
      std::optional<std::size_t> find_if(Fn &&f) const {
        for (std::size_t i = 0; i < _nodes.size(); ++i) {
          if (f(_nodes[i])) {
            return i;
          }
        }
        return std::nullopt;
      }

      std::optional<std::span<const Destination>> node_children(Destination node_dest) const {
        if (node_dest + 1 >= _destinations_lookup.size()) {
          return std::nullopt;
        }
        const std::size_t start = _destinations_lookup[node_dest];
        const std::size_t end = _destinations_lookup[node_dest + 1];
        if (start > end || end > _destinations.size()) {
          return std::nullopt;
        }
        return std::span<const Destination>(_destinations.data() + start, end - start);
      }

      std::optional<std::span<const Destination>> node_children(const Node &node_val) const {
        return find(node_val).and_then([this](auto dest) { return node_children(dest); });
      }

      std::optional<Path> find_path_reversed(std::size_t src, std::size_t dest) const {
        if (src >= _nodes.size() || dest >= _nodes.size()) {
          return std::nullopt;
        }
        if (src == dest) {
          return Path{_nodes[src]};
        }

        auto children_opt = node_children(src);
        if (!children_opt) {
          return std::nullopt;
        }
        auto &children = *children_opt;

        std::optional<Path> shortest_path = std::nullopt;
        for (auto child : children) {
          auto tmp = find_path_reversed(child, dest);
          if (tmp.has_value() &&
              !shortest_path || tmp->size() + 1 < shortest_path->size()) {
            shortest_path = std::move(tmp);
            shortest_path->emplace_back(_nodes[src]);
          }
        }

        return shortest_path;
      }

      std::optional<Path> find_path(std::size_t src, std::size_t dest) const {
        auto tmp = find_path_reversed(src, dest);
        if (tmp) {
          mr::reverse(*tmp);
        }
        return tmp;
      }

      template <typename Fn1, typename Fn2>
        requires (std::is_invocable_v<Fn1, Node> && std::is_invocable_v<Fn2, Node>)
      std::optional<Path> find_path(Fn1 &&f1, Fn2 &&f2) const {
        auto src = find_if(std::forward<Fn1>(f1));
        auto dest = find_if(std::forward<Fn2>(f2));
        if (!src || !dest) {
          return std::nullopt;
        }
        return find_path(*src, *dest);
      }

      template <typename... Args> requires(std::is_constructible_v<T, Args...>)
      Graph &add_node(Args... args) {
        _nodes.emplace_back(std::forward<Args>(args)...);
        const std::size_t required_size = _nodes.size() + 1;
        while (_destinations_lookup.size() < required_size) {
          _destinations_lookup.emplace_back(_destinations.size());
        }
        return *this;
      }

      Graph &add_edge(std::size_t src, std::size_t dest) {
        if (src >= _nodes.size() || dest >= _nodes.size()) {
          return *this;
        }

        auto children_opt = node_children(src);
        if (!children_opt) {
          return *this;
        }
        auto children = *children_opt;

        if (std::find(children.begin(), children.end(), dest) != children.end()) {
          return *this;
        }

        const std::size_t insert_pos = _destinations_lookup[src + 1];
        _destinations.emplace_at(insert_pos, dest);

        for (std::size_t i = src + 1; i < _destinations_lookup.size(); ++i) {
          ++_destinations_lookup[i];
        }

        return *this;
      }

      mr::Vector<Node> &nodes() noexcept { return _nodes; }
      const mr::Vector<Node> &nodes() const noexcept { return _nodes; }
    };
}  // namespace mr
