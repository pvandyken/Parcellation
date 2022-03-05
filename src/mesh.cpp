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

const vector<int> Mesh::getTriangleNeighbors(int triangle) {
  set<int> neighbors;
  for (auto point : polygons(triangle, all)) {
    const vector<int> &trianglesOfPoint = getTrianglesOfPoint(point);
    neighbors.insert(trianglesOfPoint.begin(), trianglesOfPoint.end());
  }
  return vector<int>(neighbors.begin(), neighbors.end());
}