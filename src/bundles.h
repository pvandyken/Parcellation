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
  Bundle(vector<vector<Vector3f>> fibers) : fibers{fibers} {};
  vector<vector<Vector3f>> fibers;
};