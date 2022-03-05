#pragma once
#include <Eigen/Dense>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;
using namespace Eigen;

class Bundle {
 public:
  Bundle(const string &path);
  vector<vector<Vector3f>> fibers;
};