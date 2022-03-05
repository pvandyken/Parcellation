#pragma once
#include <math.h>

#include <Eigen/Dense>
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <vector>

using namespace Eigen;

using namespace std;

namespace Orientation {
bool isAligned(const vector<Vector3f> &fiber,
               const vector<Vector3f> &reference);
void alignOrientation(vector<vector<Vector3f>> &fibers);
}  // namespace Orientation