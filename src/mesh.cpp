#include "mesh.h"

const vector<int> &Mesh::getTrianglesOfPoint(int point) {
  if (this->trianglesOfPointsIndex.size() == 0) {
    // Initialize the index according to the number of points
    trianglesOfPointsIndex.resize(this->vertices.rows());
    // Loop through every triangle
    for (size_t i = 0; i < this->polygons.rows(); i++) {
      for (auto point : polygons(i, all)) {
        trianglesOfPointsIndex[point].push_back(i);
      }
    }
  }

  return this->trianglesOfPointsIndex[point];
}

const vector<int> Mesh::getTrianglesOfPoint(VectorXi points) {
  set<int> triangles;
  for (auto point : points) {
    const vector<int> &trianglesOfPoint = getTrianglesOfPoint(point);
    triangles.insert(trianglesOfPoint.begin(), trianglesOfPoint.end());
  }
  return vector<int>(triangles.begin(), triangles.end());
}

const vector<int> Mesh::getTriangleNeighbors(int triangle) {
  return getTrianglesOfPoint(polygons(triangle, all));
}
const vector<int> Mesh::getTriangleNeighbors(vector<int> triangles) {
  unordered_set<int> points;
  for (auto triangle : triangles) {
    for (auto point : polygons(triangle, all)) {
      points.insert(point);
    }
  }
  vector<int> point_v(points.begin(), points.end());
  return getTrianglesOfPoint(Map<VectorXi>(point_v.data(), point_v.size()));
}

const vector<int> Mesh::getTriangleNeighbors(vector<int> triangles,
                                             vector<int> exclude) {
  unordered_set<int> included;
  unordered_set<int> excluded(exclude.begin(), exclude.end());
  for (auto triangle : triangles) {
    if (excluded.find(triangle) != excluded.end()) {
      included.insert(triangle);
    }
  }
  vector<int> included_v(included.begin(), included.end());
  return getTrianglesOfPoint(
      Map<VectorXi>(included_v.data(), included_v.size()));
}