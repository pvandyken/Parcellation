#pragma once

#include "math.h"
#include "ndarray.h"
#include "utils.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

namespace Intersection {
    const float dotProduct(const float a[], float *&b);
    float *crossProduct(const float a[], const float b[]);
    bool ray_triangle_intersection(const float ray_near[], const float ray_dir[],
                                const float Points[][3], float &t);

    vector<vector<float>> multiple_vertices(float *triangle[3]);
    vector<vector<vector<float>>>
    multiple_triangles(float *triangles[1][3], int &len, const int polys[][3]);
    vector<vector<vector<float>>> segment_triangles(float *triangles[1][3]);
    vector<vector<float>>
    get_triangle_centroid(vector<vector<vector<float>>> triangles, const int &N);

    const bool getMeshAndFiberEndIntersection(
        Vector3f &fiberP0, Vector3f &fiberP1, const int &nPoints,
        const int &nPtsLine, const int &N, const int &npbp,
        vector<vector<float>> &index, const float &step,
        vector<vector<vector<bool>>> &cubeNotEmpty,
        const vector<vector<vector<vector<int>>>> &centroidIndex,
        const vector<vector<vector<vector<vector<float>>>>> &almacen,
        Ndarray<float> &vertex, Ndarray<int> &polygons, int &Ind,
        vector<float> &ptInt);

    const bool getMeshAndFiberIntersection(
        vector<Vector3f> &fiber, const int &nPoints, const int &nPtsLine,
        const int &N, const int &npbp, vector<vector<float>> &index,
        const float &step, vector<vector<vector<bool>>> &cubeNotEmpty,
        const vector<vector<vector<vector<int>>>> &centroidIndex,
        const vector<vector<vector<vector<vector<float>>>>> &almacen,
        Ndarray<float> &vertex, Ndarray<int> &polygons, int &InInd,
        int &FnInd, vector<float> &InPtInt, vector<float> &FnPtInt);

    void meshAndBundlesIntersection(
        Ndarray<float> vertex, Ndarray<int> polygons,
        vector<vector<vector<Vector3f>>> &Points,
        const int &nPtsLine, vector<vector<int>> &InTri, vector<vector<int>> &FnTri,
        vector<vector<vector<float>>> &InPoints,
        vector<vector<vector<float>>> &FnPoints, vector<vector<int>> &fib_index);
}