#pragma once
#include <vector>
#include <stdexcept>
#include <math.h>
#include <algorithm>
#include <iostream>
#include <Eigen/Dense>

using namespace Eigen;

using namespace std;

namespace Orientation {
    bool isAligned(const vector<Vector3f> &fiber, const vector<Vector3f> &reference);
    void alignOrientation(vector<vector<Vector3f>> &fibers);
}