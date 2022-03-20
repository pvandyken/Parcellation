#pragma once

#include <string>
#include <vector>

#include <Eigen/Dense>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

#include "types.h"

namespace py = pybind11;

class Mesh {
 private:
  py::array vertexData;
  py::array polygonData;

  std::vector<std::vector<int>> trianglesOfPointsIndex;
  const std::vector<int> getTrianglesOfPoint(Eigen::VectorXi points);

 public:
  Mesh() : vertices{Eigen::MatrixX3f()}, polygons{Eigen::MatrixX3i()} {};
  Mesh(const std::string& name);

  EigenDRef<const Eigen::MatrixX3f> vertices;
  EigenDRef<const Eigen::MatrixX3i> polygons;

  const std::vector<int> getTriangleNeighbors(int triangle,
                                              bool edgeAdjacent = false);
  const std::vector<int> getTriangleNeighbors(std::vector<int> triangles);
  const std::vector<int> getTriangleNeighbors(std::vector<int> triangles,
                                              std::vector<int> exclude);

  const std::vector<int>& getTrianglesOfPoint(int point);
  const std::vector<int> getTrianglesOfPoint(std::vector<int> points);

  static Mesh from_file(const std::string& name);
};