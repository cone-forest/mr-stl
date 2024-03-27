#pragma once

#include <functional>
#include "mr-stl/vector/vector.hpp"
#include "mr-stl/vector/amortized_vector.hpp"

namespace mr {
  template <typename T>
    class Graph {
    public:
      using Node = T;
      using Destination = std::size_t;
      using Edge = std::pair<Destination, Destination>;
      using Path = mr::Vector<Node>;

    private:
      // a flat multimap of edges
      mr::Vector<Destination> _destinations;
      mr::Vector<Destination> _destinations_lookup;
      // stored data
      mr::Vector<Node> _nodes;

    public:
      std::optional<std::size_t> find(const T &node) const {
        for (int i = 0; i < _nodes.size(); i++) {
          if (_nodes[i] == node) {
            return i;
          }
        }
        return std::nullopt;
      }

      template <typename Fn> requires (std::is_invocable_v<Fn, Node>)
      std::optional<std::size_t> find_if(Fn &&f) const {
        for (int i = 0; i < _nodes.size(); i++) {
          if (f(_nodes[i])) {
            return i;
          }
        }
        return std::nullopt;
      }

      std::optional<std::span<Destination>> node_children(Destination node_dest) {
        Destination * const beg = _destinations_lookup[node_dest];
        Destination * const end = _destinations_lookup[node_dest + 1] - 1;
        return std::span<Destination>(beg, end);
      }

      std::optional<std::span<Destination>> node_children(const Node &node_val) {
        return find(node_val).and_then(bind_self(&Graph::node_children, this));
      }

      std::optional<Path> find_path_reversed(std::size_t src, std::size_t dest) const {
        if (src > _nodes.size() || dest > _nodes.size()) {
          return std::nullopt;
        }
        if (src == dest) {
          return Path{dest};
        }

        // find range of edges where src is source
        std::span<Destination> children = node_children(src);

        // calculate path from each child-node, return minimum
        Path candidate;
        for (auto child : children) {
          auto tmp = find_path_reversed(child, dest);
          if (tmp.has_value() && (tmp->size() < candidate.size() || candidate.size() == 0)) {
            candidate = std::move(tmp.value());
          }
        }

        return std::move(candidate.emplace_back(src));
      }

      std::optional<Path> find_path(std::size_t src, std::size_t dest) const {
        auto tmp = find_path_reversed(src, dest);
        return tmp.and_then([](Path &path) {
            std::ranges::reverse(path);
            return path;
            });
      }

      template <typename Fn1, typename Fn2>
        requires (std::is_invocable_v<Fn1, Node> && std::is_invocable_v<Fn2, Node>)
      std::optional<Path> find_path(Fn1 &&f1, Fn2 &&f2) const {
        return and_then(find_if(f1), find_if(f2), bind_self(&Graph::find_path, this));
      }

      template <typename... Args> requires(std::is_constructible_v<T, Args...>)
      Graph &add_node(Args... args) {
        _nodes.emplace_back(args...);
        return *this;
      }

      Graph &add_edge(std::size_t src, std::size_t dest) {
        std::span<Destination> children = node_children(src);
        if (contains(children, [dest](auto val) { return val == dest; })) {
          return *this;
        }

        _destinations.emplace_at(&*children.end(), dest);
        for (std::size_t i = src; i < _destinations_lookup.size(); i++) {
          _destinations_lookup[i]++;
        }

        return *this;
      }

      template <typename Func1, typename Func2>
        requires(std::is_invocable_v<Func1, Node> && std::is_invocable_v<Func2, Node>)
      Graph &add_edge(Func1 &&f1, Func2 &&f2) {
        return and_then(find_if(f1), find_if(f2), bind_self(&Graph::add_edge, this));
      }

      // getters
      mr::Vector<Node> &nodes() noexcept { return _nodes; }
      const mr::Vector<Node> &nodes() const noexcept { return _nodes; }
    };
}  // namespace mr
