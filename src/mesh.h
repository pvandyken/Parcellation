#pragma once
#include <Eigen/Dense>
#include <cppitertools/combinations.hpp>
#include <cppitertools/filter.hpp>
#include <set>
#include <algorithm>
#include <unordered_set>
#include <vector>

#include "types.h"


class Mesh {
 public:
  Mesh() : vertices{Eigen::MatrixX3f()}, polygons{Eigen::MatrixX3i()} {};
  Mesh(const EigenDRef<const Eigen::MatrixX3f> vertices,
       const EigenDRef<const Eigen::MatrixX3i> polygons)
      : vertices{vertices}, polygons{polygons} { };

  const EigenDRef<const Eigen::MatrixX3f> vertices;
  const EigenDRef<const Eigen::MatrixX3i> polygons;

  const std::vector<int> getTriangleNeighbors(int triangle,
                                         bool edgeAdjacent = false);
  const std::vector<int> getTriangleNeighbors(std::vector<int> triangles);
  const std::vector<int> getTriangleNeighbors(std::vector<int> triangles,
                                         std::vector<int> exclude);

  const std::vector<int> &getTrianglesOfPoint(int point);
  const std::vector<int> getTrianglesOfPoint(std::vector<int> points);

 private:
  std::vector<std::vector<int>> trianglesOfPointsIndex;
  const std::vector<int> getTrianglesOfPoint(Eigen::VectorXi points);
};