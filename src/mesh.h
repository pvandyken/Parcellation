#pragma once

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>

#include <Eigen/Dense>
#include <string>
#include <vector>

#include "types.h"

namespace py = pybind11;

class Mesh {
 public:
  Mesh() : vertices{Eigen::MatrixX3f()}, polygons{Eigen::MatrixX3i()} {};
  Mesh(const std::string& name);
  Mesh(const Eigen::MatrixX3f vertices, const Eigen::MatrixX3i polygons)
      : vertices{vertices}, polygons{polygons} {};

  const Eigen::MatrixX3f vertices;
  const Eigen::MatrixX3i polygons;

  const Mesh filterTriangles(std::vector<int> whitelist);

  py::object getPolydata();

  void saveVtk(const std::string path, bool binary = false);

  const std::vector<int> getTriangleNeighbors(int triangle,
                                              bool edgeAdjacent = false);
  const std::vector<int> getTriangleNeighbors(std::vector<int> triangles);
  const std::vector<int> getTriangleNeighbors(std::vector<int> triangles,
                                              std::vector<int> exclude);

  const std::vector<int>& getTrianglesOfPoint(int point);
  const std::vector<int> getTrianglesOfPoint(std::vector<int> points);

  static Mesh from_file(const std::string& name);

 private:
  std::vector<std::vector<int>> trianglesOfPointsIndex;
  const std::vector<int> getTrianglesOfPoint(Eigen::VectorXi points);
};