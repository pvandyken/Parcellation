#include "mesh.h"

#include <pybind11/eigen.h>
#include <pybind11/pybind11.h>

#include <algorithm>
#include <cppitertools/chain.hpp>
#include <cppitertools/combinations.hpp>
#include <cppitertools/enumerate.hpp>
#include <cppitertools/filter.hpp>
#include <cppitertools/range.hpp>
#include <unordered_set>

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
    : vertices{get_nib_data(name, "pointset").cast<MatrixF>()},
      polygons{get_nib_data(name, "triangle").cast<MatrixI>()} {}

Mesh::Mesh(const std::filesystem::path name)
    : vertices{get_nib_data(name.lexically_normal().string(), "pointset")
                   .cast<MatrixF>()},
      polygons{get_nib_data(name.lexically_normal().string(), "triangle")
                   .cast<MatrixI>()} {}

const Mesh Mesh::filterTriangles(std::vector<int> whitelist) {
  using Eigen::all;

  // Remove duplicates from triangles
  std::sort(whitelist.begin(), whitelist.end());
  whitelist.erase(std::unique(whitelist.begin(), whitelist.end()),
                  whitelist.end());

  // Get the triangles we want
  Eigen::MatrixX3i triangles(whitelist.size(), 3);
  for (auto&& i : iter::range(whitelist.size())) {
    triangles(i, all) = this->polygons(whitelist[i], all);
  }

  // Find the points referenced by the triangles, and remove duplicates
  auto neededPointChain = iter::chain.from_iterable(triangles.rowwise());
  auto neededPoints =
      std::vector<int>(neededPointChain.begin(), neededPointChain.end());
  std::sort(neededPoints.begin(), neededPoints.end());
  neededPoints.erase(std::unique(neededPoints.begin(), neededPoints.end()),
                     neededPoints.end());

  // Reindex the triangles as 0, 1, 2,...
  std::unordered_map<int, int> pointsIndex;
  for (auto&& [i, point] : iter::enumerate(neededPoints)) {
    pointsIndex[point] = i;
  }
  // Transfer the new indices to our triangle array
  for (auto triangle : triangles.rowwise()) {
    for (auto& x : triangle) {
      x = pointsIndex[x];
    }
  }

  // Get the needed points as a new object
  Eigen::MatrixX3f points(neededPoints.size(), 3);
  for (auto&& [i, point] : iter::enumerate(neededPoints)) {
    points(i, all) = this->vertices(point, all);
  }

  return Mesh(points, triangles);
}

py::object Mesh::getPolydata() {
  py::module_ futil = py::module_::import("fury.utils");
  py::object PolyData = futil.attr("PolyData");
  py::object set_pd_triangles = futil.attr("set_polydata_triangles");
  py::object set_pd_vertices = futil.attr("set_polydata_vertices");
  py::object pd = PolyData();
  set_pd_triangles(pd, this->polygons);
  set_pd_vertices(pd, this->vertices);
  return pd;
}

void Mesh::saveVtk(const std::variant<std::string, std::filesystem::path> path,
                   bool binary) {
  using namespace pybind11::literals;
  std::string name;
  if (auto str_ptr = std::get_if<std::string>(&path)) {
    name = *str_ptr;
  } else {
    auto path_ptr = std::get<std::filesystem::path>(path);
    name = path_ptr.lexically_normal().string();
  }
  py::object save_polydata =
      py::module_::import("fury.io").attr("save_polydata");
  save_polydata(this->getPolydata(), name, "binary"_a = binary);
}

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
