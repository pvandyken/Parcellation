#pragma once

#include <vector>
#include <map>
#include "mesh.h"

using namespace std;


class BundleIntersections {
public:
    BundleIntersections()
        : triangles{vector<vector<int>>()},
          points{vector<vector<vector<float>>>()} {};
    BundleIntersections(const int size)
        : triangles{vector<vector<int>>(size)},
          points{vector<vector<vector<float>>>(size)} {};
    BundleIntersections(vector<vector<int>> triangles, vector<vector<vector<float>>> points)
        : triangles{triangles}, points{points} {};

    uint16_t id;
    vector<vector<int>> triangles;
    vector<vector<vector<float>>> points;

    map<int, float> getTriangleProbabilities(int bundleId, Mesh &mesh, const vector<map<int, int>> &triangleIntersections);
};