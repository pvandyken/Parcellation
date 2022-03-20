#include <algorithm>
#include <unordered_set>

#include <cppitertools/combinations.hpp>
#include <cppitertools/filter.hpp>
#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>

#include "mesh.h"

typedef Eigen::MatrixX3i MatrixI;
typedef Eigen::MatrixX3f MatrixF;

namespace {
py::array get_nib_data(const std::string& path, const std::string& item) {
  py::module_ nib = py::module_::import("nibabel");
  py::object data = nib.attr("load")(path);

  return data.attr("agg_data")(item);
}
}  // namespace

Mesh::Mesh(const std::string& name)
    : vertexData{get_nib_data(name, "triangle")},
      polygonData{get_nib_data(name, "pointset")},
      vertices{vertexData.cast<EigenDRef<const MatrixF>>()},
      polygons{polygonData.cast<EigenDRef<const MatrixI>>()} {}

const std::vector<int>& Mesh::getTrianglesOfPoint(int point) {
  if (this->trianglesOfPointsIndex.size() == 0) {
    // Initialize the index according to the number of points
    trianglesOfPointsIndex.resize(this->vertices.rows());
    // Loop through every triangle
    for (size_t i = 0; i < this->polygons.rows(); i++) {
      for (auto point : polygons(i, Eigen::all)) {
        trianglesOfPointsIndex[point].push_back(i);
      }
    }
  }

  return this->trianglesOfPointsIndex[point];
}

const std::vector<int> Mesh::getTrianglesOfPoint(Eigen::VectorXi points) {
  std::unordered_set<int> triangles;
  for (auto point : points) {
    const std::vector<int>& trianglesOfPoint = getTrianglesOfPoint(point);
    triangles.insert(trianglesOfPoint.begin(), trianglesOfPoint.end());
  }
  return std::vector<int>(triangles.begin(), triangles.end());
}

const std::vector<int> Mesh::getTrianglesOfPoint(std::vector<int> points) {
  return getTrianglesOfPoint(
      Eigen::Map<Eigen::VectorXi>(points.data(), points.size()));
}

const std::vector<int> Mesh::getTriangleNeighbors(int triangle,
                                                  bool edgeAdjacent) {
  if (!edgeAdjacent) {
    return getTrianglesOfPoint(polygons(triangle, Eigen::all));
  }

  // Get adjacents to each point
  std::vector<std::vector<int>> adj_triangles;
  for (auto i : polygons(triangle, Eigen::all)) {
    // Discard the triangle at the center...
    auto filtered = iter::filter([triangle](int i) { return i != triangle; },
                                 getTrianglesOfPoint(i));
    adj_triangles.emplace_back(filtered.begin(), filtered.end());
  }

  // Find the triangle commonly adjacent to each pair of points
  std::vector<int> neighbors;
  for (auto&& i : iter::combinations(adj_triangles, 2)) {
    std::sort(i[0].begin(), i[0].end());
    std::sort(i[1].begin(), i[1].end());
    std::set_intersection(i[0].begin(), i[0].end(), i[1].begin(), i[1].end(),
                          std::back_inserter(neighbors));
  }

  return neighbors;
}

const std::vector<int> Mesh::getTriangleNeighbors(std::vector<int> triangles) {
  std::unordered_set<int> points;
  for (auto triangle : triangles) {
    for (auto point : polygons(triangle, Eigen::all)) {
      points.insert(point);
    }
  }
  return getTrianglesOfPoint(std::vector<int>(points.begin(), points.end()));
}

const std::vector<int> Mesh::getTriangleNeighbors(std::vector<int> triangles,
                                                  std::vector<int> exclude) {
  std::unordered_set<int> included;
  std::unordered_set<int> excluded;
  for (auto triangle : exclude) {
    for (auto point : polygons(triangle, Eigen::all)) {
      excluded.insert(point);
    }
  }
  for (auto triangle : triangles) {
    for (auto point : polygons(triangle, Eigen::all)) {
      if (excluded.find(point) == excluded.end()) {
        included.insert(point);
      }
    }
  }
  return getTrianglesOfPoint(
      std::vector<int>(included.begin(), included.end()));
}
