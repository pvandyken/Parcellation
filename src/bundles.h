#pragma once
#include <Eigen/Dense>
#include <vector>
#include <iostream>
#include <fstream>

using namespace std;
using namespace Eigen;

class Bundle {
public:
    Bundle(const string &path);
    vector<vector<Vector3f>> fibers;

    vector<int> inTri;
    vector<int> fnTri;
    vector<vector<float>> inPoints;
    vector<vector<float>> fnPoints;
    vector<int> fibIndex;
};