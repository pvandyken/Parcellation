#pragma once
#include <Eigen/Dense>
#include "types.h"
#include <vector>
#include <set>

using namespace std;
using namespace Eigen;

class Mesh {
public:
    Mesh(
        const EigenDRef<const MatrixX3f> vertices,
        const EigenDRef<const MatrixX3i> polygons
    ) : vertices{vertices}, polygons{polygons} {
        trianglesOfPointsIndex = vector<vector<int>>();
    };

    const EigenDRef<const MatrixX3f> vertices;
    const EigenDRef<const MatrixX3i> polygons;

    const vector<int>& getTrianglesOfPoint(int point);
    const vector<int> getTriangleNeighbors(int triangle);

private:
    vector<vector<int>> trianglesOfPointsIndex;
};