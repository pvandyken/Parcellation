#pragma once

#include <vector>
#include <map>
#include "mesh.h"

using namespace std;


class BundleIntersections {
public:
    BundleIntersections()
        : triangles{vector<int>()},
          points{vector<vector<float>>()} {};
    BundleIntersections(const int size)
        : triangles{vector<int>(size)},
          points{vector<vector<float>>(size)} {};
    BundleIntersections(vector<int> triangles, vector<vector<float>> points)
        : triangles{triangles}, points{points} {};

    uint16_t id;
    vector<int> triangles;
    vector<vector<float>> points;

    map<int, float> getTriangleProbabilities(Mesh &mesh, const vector<map<int, int>> &triangleIntersections);

    static void fromRange(size_t size, vector<BundleIntersections> &arr);
};