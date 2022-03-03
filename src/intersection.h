#pragma once

#include "math.h"
#include "utils.h"
#include "mesh.h"
#include "types.h"
#include "bundles.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <Eigen/Dense>


using namespace std;
using namespace Eigen;

namespace Intersection {
    const float dotProduct(const float a[], float *&b);
    float *crossProduct(const float a[], const float b[]);
    bool ray_triangle_intersection(const float ray_near[], const float ray_dir[],
                                const float Points[][3], float &t);

    vector<vector<float>> multiple_vertices(const float *triangle[3]);
    vector<vector<vector<float>>>
    multiple_triangles(const float *triangles[1][3], int &len, const int polys[][3]);
    vector<vector<vector<float>>> segment_triangles(const float *triangles[1][3]);
    vector<vector<float>>
    get_triangle_centroid(vector<vector<vector<float>>> triangles, const int &N);
}


class CorticalIntersection {
public:
    CorticalIntersection(
        const Mesh &mesh,
        vector<Bundle> &bundles, const int &nPtsLine);

    CorticalIntersection(const Mesh &mesh, vector<vector<int>> &interceptions)
        : mesh{mesh}, inTri{interceptions} {};

    const Mesh &mesh;
    vector<vector<int>> inTri;
    vector<vector<int>> fnTri;
    vector<vector<vector<float>>> inPoints;
    vector<vector<vector<float>>> fnPoints;
    vector<vector<int>> fibIndex;

    map<int, int> &getTrianglesIntersected(const int index);

private:
    vector<map<int, int>> trianglesIntersected;
    const bool getMeshAndFiberEndIntersection(
        Vector3f &fiberP0, Vector3f &fiberP1, const int &nPoints,
        const int &nPtsLine, const int &N, const int &npbp,
        vector<vector<float>> &index, const float &step,
        vector<vector<vector<bool>>> &cubeNotEmpty,
        const vector<vector<vector<vector<int>>>> &centroidIndex,
        const vector<vector<vector<vector<vector<float>>>>> &almacen,
        const EigenDRef<const MatrixX3f> &vertex, const EigenDRef<const MatrixX3i> &polygons, int &Ind,
        vector<float> &ptInt);

    const bool getMeshAndFiberIntersection(
        vector<Vector3f> &fiber, const int &nPoints, const int &nPtsLine,
        const int &N, const int &npbp, vector<vector<float>> &index,
        const float &step, vector<vector<vector<bool>>> &cubeNotEmpty,
        const vector<vector<vector<vector<int>>>> &centroidIndex,
        const vector<vector<vector<vector<vector<float>>>>> &almacen,
        const EigenDRef<const MatrixX3f> &vertex, const EigenDRef<const MatrixX3i> &polygons, int &InInd,
        int &FnInd, vector<float> &InPtInt, vector<float> &FnPtInt);

};
