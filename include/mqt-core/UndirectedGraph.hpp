//
// This file is part of the MQT QMAP library released under the MIT license.
// See README.md or go to https://github.com/cda-tum/mqt-qmap for more
// information.
//

#pragma once

#include "Definitions.hpp"

#include <numeric>
#include <sstream>
#include <unordered_set>

namespace qc {

template <class V, class E> class UndirectedGraph {
protected:
  // the adjecency matrix works with indices
  std::vector<std::vector<E>> adjacencyMatrix{};
  // the mapping of vertices to indices in the graph are stored in a map
  std::unordered_map<V, std::size_t> mapping;
  // the inverse mapping is used to get the vertex from the index
  std::unordered_map<std::size_t, V> invMapping;
  // the number of vertices in the graph
  std::size_t nVertices = 0;
  // the number of edges in the graph
  std::size_t nEdges = 0;
  // the degrees of the vertices in the graph
  std::vector<std::size_t> degrees;

public:
  auto addVertex(V v) -> void {
    // check whether the vertex is already in the graph, if so do nothing
    if (mapping.find(v) == mapping.end()) {
      mapping[v] = nVertices;
      invMapping[nVertices] = v;
      ++nVertices;
      for (auto& row : adjacencyMatrix) {
        row.emplace_back(nullptr);
      }
      adjacencyMatrix.emplace_back(1, nullptr);
      degrees.emplace_back(0);
    } else {
      std::stringstream ss;
      ss << "The vertex " << v << " is already in the graph.";
      throw std::invalid_argument(ss.str());
    }
  }
  auto addEdge(V u, V v, E e) -> void {
    if (mapping.find(u) == mapping.end()) {
      addVertex(u);
    }
    if (mapping.find(v) == mapping.end()) {
      addVertex(v);
    }
    const auto i = mapping.at(u);
    const auto j = mapping.at(v);
    if (i < j) {
      if (adjacencyMatrix[i][j - i] == nullptr) {
        ++degrees[i];
        if (i != j) {
          ++degrees[j];
        }
        ++nEdges;
      }
      adjacencyMatrix[i][j - i] = e;
    } else {
      if (adjacencyMatrix[j][i - j] == nullptr) {
        ++degrees[i];
        if (i != j) {
          ++degrees[j];
        }
        ++nEdges;
      }
      adjacencyMatrix[j][i - j] = e;
    }
  }
  [[nodiscard]] auto getNVertices() const -> std::size_t { return nVertices; }
  [[nodiscard]] auto getNEdges() const -> std::size_t { return nEdges; }
  [[nodiscard]] auto getEdge(V v, V u) const -> E {
    const auto i = mapping.at(v);
    const auto j = mapping.at(u);
    if (i < j ? adjacencyMatrix[i][j - i] != nullptr
              : adjacencyMatrix[j][i - j] != nullptr) {
      return i < j ? adjacencyMatrix[i][j - i] : adjacencyMatrix[j][i - j];
    }
    std::stringstream ss;
    ss << "The edge (" << v << ", " << u << ") does not exist.";
    throw std::invalid_argument(ss.str());
  }
  [[nodiscard]] auto getAdjacentEdges(V v) const -> std::unordered_set<std::pair<V, V>, PairHash<V>> {
    if (mapping.find(v) == mapping.end()) {
      std::stringstream ss;
      ss << "The vertex " << v << " is not in the graph.";
      throw std::invalid_argument(ss.str());
    }
    const auto i = mapping.at(v);
    std::unordered_set<std::pair<V, V>, PairHash<V>> result;
    for (std::size_t j = 0; j < nVertices; ++j) {
      if (i < j ? adjacencyMatrix[i][j - i] != nullptr
                : adjacencyMatrix[j][i - j] != nullptr) {
        const auto u = invMapping.at(j);
        result.emplace(std::make_pair(v, u));
      }
    }
    return result;
  }
  [[nodiscard]] auto getNeighbours(V v) const -> std::unordered_set<V> {
    if (mapping.find(v) == mapping.end()) {
      std::stringstream ss;
      ss << "The vertex " << v << " is not in the graph.";
      throw std::invalid_argument(ss.str());
    }
    const auto i = mapping.at(v);
    std::unordered_set<V> result;
    for (std::size_t j = 0; j < nVertices; ++j) {
      if (i < j ? adjacencyMatrix[i][j - i] != nullptr
                : adjacencyMatrix[j][i - j] != nullptr) {
        result.emplace(invMapping.at(j));
      }
    }
    return result;
  }
  [[nodiscard]] auto getDegree(V v) const -> std::size_t {
    if (mapping.find(v) == mapping.end()) {
      std::stringstream ss;
      ss << "The vertex " << v << " is not in the graph.";
      throw std::invalid_argument(ss.str());
    }
    const auto i = mapping.at(v);
    return degrees[i];
  }
  [[nodiscard]] auto getVertices() const -> std::unordered_set<V> {
    return std::accumulate(mapping.cbegin(), mapping.cend(),
                           std::unordered_set<V>(),
                           [](auto& acc, const auto& v) {
                             acc.emplace(v.first);
                             return acc;
                           });
  }
  [[nodiscard]] auto isAdjacent(const V u, const V v) const -> bool {
    const auto i = mapping.at(u);
    const auto j = mapping.at(v);
    return (i < j and adjacencyMatrix[i][j - i] != nullptr) or
           (j < i and adjacencyMatrix[j][i - j] != nullptr);
  }
  [[nodiscard]] auto
  // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
  isAdjacentEdge(const std::pair<V, V>& e, const std::pair<V, V>& f) const
      -> bool {
    return e.first == f.first or e.first == f.second or e.second == f.first or
           e.second == f.second;
  }
  /// Outputs a string representation of the graph in the DOT format
  [[nodiscard]] auto toString() const -> std::string {
    std::stringstream ss;
    ss << "graph {\n";
    for (const auto& [v, i] : mapping) {
      ss << "  " << i << " [label=\"" << v << "\"];\n";
    }
    for (std::size_t i = 0; i < nVertices; ++i) {
      for (std::size_t j = i + 1; j < nVertices; ++j) {
        if (adjacencyMatrix[i][j - i] != nullptr) {
          ss << "  " << i << " -- " << j << ";\n";
        }
      }
    }
    ss << "}\n";
    return ss.str();
  }
  friend auto operator<<(std::ostream& os, const UndirectedGraph& g)
      -> std::ostream& {
    os << g.toString(); // Using toString() method
    return os;
  }
};
} // namespace qc
