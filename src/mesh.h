#pragma once
#include <Eigen/Dense>
#include <set>
#include <vector>

#include "types.h"

using namespace std;
using namespace Eigen;

class Mesh {
 public:
  Mesh() : vertices{MatrixX3f()}, polygons{MatrixX3i()} {};
  Mesh(const EigenDRef<const MatrixX3f> vertices,
       const EigenDRef<const MatrixX3i> polygons)
      : vertices{vertices}, polygons{polygons} {
    trianglesOfPointsIndex = vector<vector<int>>();
  };

  const EigenDRef<const MatrixX3f> vertices;
  const EigenDRef<const MatrixX3i> polygons;

  const vector<int> &getTrianglesOfPoint(int point);
  const vector<int> getTriangleNeighbors(int triangle);

 private:
  vector<vector<int>> trianglesOfPointsIndex;
};