#pragma once
#include <Eigen/Dense>
#include "types.h"
#include <vector>

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

    vector<int>& getTrianglesOfPoint(int point);

private:
    vector<vector<int>> trianglesOfPointsIndex;
};