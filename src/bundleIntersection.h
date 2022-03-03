#pragma once

#include <vector>

using namespace std;


class BundleIntersection {
public:
    BundleIntersection() : triangles{vector<int>()}, points{vector<vector<float>>()} {};
    BundleIntersection(vector<int> triangles, vector<vector<float>> points)
        : triangles{triangles}, points{points} {};
    vector<int> triangles;
    vector<vector<float>> points;
};