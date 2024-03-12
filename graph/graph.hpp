#pragma once

#include <ranges>
#include "vector/vector.hpp"
#include "vector/amortized_vector.hpp"

namespace mr {
  template <typename T>
    class Graph {
    public:
      using Node = T;
      using Edge = std::pair<std::size_t, std::size_t>;
      using Path = mr::Vector<Node>;

    private:
      mr::Vector<Edge> _edges;
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

      std::optional<Path> find_path_reversed(std::size_t src, std::size_t dest) const {
        if (src > _nodes.size() || dest > _nodes.size()) {
          return std::nullopt;
        }
        if (src == dest) {
          return Path{dest};
        }

        // find range of edges where src is source
        const Edge * beg = nullptr;
        for (const Edge &edge : _edges) {
          if (edge.first == src) {
            beg = &edge;
            break;
          }
        }
        // calculate path from each child-node, return minimum
        Path candidate;
        while (beg->first == src) {
          auto tmp = find_path_reversed(beg->second, dest);
          if (tmp.has_value() && (tmp->size() < candidate.size() || candidate.size() == 0)) {
            candidate = std::move(tmp.value());
          }
          beg++;
        }

        return std::move(candidate.emplace_back(src));
      }

      std::optional<Path> find_path(std::size_t src, std::size_t dest) const {
        auto tmp = find_path_reversed(src, dest);
        if (tmp.has_value()) {
          // return std::ranges::reverse(std::move(tmp.value()));
          std::ranges::reverse(tmp.value());
          return std::move(tmp.value());
        }
        return std::nullopt;
      }

      template <typename Fn1, typename Fn2>
        requires (std::is_invocable_v<Fn1, Node> && std::is_invocable_v<Fn2, Node>)
      std::optional<Path> find_path(Fn1 &&f1, Fn2 &&f2) const {
        std::optional<std::size_t> src = find_if(f1);
        std::optional<std::size_t> dest = find_if(f2);

        if (src.has_value() &&
            dest.has_value()) [[likely]] {
          return find_path(src.value(), dest.value());
        }
        return std::nullopt;
      }

      template <typename... Args> requires(std::is_constructible_v<T, Args...>)
      Graph &add_node(Args... args) {
        _nodes.emplace_back(args...);
        return *this;
      }

      Graph &add_edge(std::size_t src, std::size_t dest) {
        _edges.push_sorted(src, dest);
        return *this;
      }

      template <typename Func1, typename Func2>
        requires(std::is_invocable_v<Func1, Node> && std::is_invocable_v<Func2, Node>)
      Graph &add_edge(Func1 &&f1, Func2 &&f2) {
        std::optional<std::size_t> src = find_if(f1);
        std::optional<std::size_t> dest = find_if(f2);

        if (src && dest) [[likely]] {
          add_edge(src.value(), dest.value());
        }

        return *this;
      }

      // getters
      mr::Vector<Node> &nodes() noexcept { return _nodes; }

      const mr::Vector<Node> &nodes() const noexcept { return _nodes; }

      mr::Vector<Edge> &edges() noexcept { return _edges; }

      const mr::Vector<Edge> &edges() const noexcept { return _edges; }
    };
}  // namespace mr
