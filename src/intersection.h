#pragma once

#include <Eigen/Dense>
#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "bundleIntersection.h"
#include "bundles.h"
#include "io.h"
#include "orientation.h"
#include "math.h"
#include "mesh.h"
#include "types.h"
#include "utils.h"

using namespace std;
using namespace Eigen;

class CorticalIntersection {
 public:
  CorticalIntersection() : mesh{Mesh()} {};
  CorticalIntersection(const Mesh &mesh, vector<Bundle> &bundles,
                       const int &nPtsLine);

  CorticalIntersection(const Mesh &mesh,
                       vector<BundleIntersections> &interceptions)
      : mesh{mesh}, front{interceptions} {};

  vector<BundleIntersections> front;
  vector<BundleIntersections> back;
  vector<vector<int>> fibIndex;
  const Mesh &mesh;

  const vector<map<int, int>> &getTrianglesIntersected(
      fn<void> const &sigintHander = []() {});

  const vector<vector<int>> getTrianglesFront();
  const vector<vector<int>> getTrianglesBack();

  static CorticalIntersection fromBundles(
    const EigenDRef<const MatrixX3f> vertices,
    const EigenDRef<const MatrixX3i> polygons, string bundle_dir,
    const int nPtsLine);

 private:
  vector<map<int, int>> trianglesIntersected;

  const vector<vector<int>> getTriangles(
      vector<BundleIntersections> const &intersections);

  const bool getMeshAndFiberEndIntersection(
      Vector3f &fiberP0, Vector3f &fiberP1, const int &distanceFactor,
      const int &nPtsLine, const int &N, const int &npbp,
      vector<vector<float>> &index, const float &step,
      vector<vector<vector<bool>>> &cubeNotEmpty,
      const vector<vector<vector<vector<int>>>> &centroidIndex,
      const vector<vector<vector<vector<vector<float>>>>> &almacen,
      const EigenDRef<const MatrixX3f> &vertex,
      const EigenDRef<const MatrixX3i> &polygons, int &Ind,
      vector<float> &ptInt);

  const bool getMeshAndFiberIntersection(
      vector<Vector3f> &fiber, const int &nPoints, const int &nPtsLine,
      const int &N, const int &npbp, vector<vector<float>> &index,
      const float &step, vector<vector<vector<bool>>> &cubeNotEmpty,
      const vector<vector<vector<vector<int>>>> &centroidIndex,
      const vector<vector<vector<vector<vector<float>>>>> &almacen,
      const EigenDRef<const MatrixX3f> &vertex,
      const EigenDRef<const MatrixX3i> &polygons, int &InInd, int &FnInd,
      vector<float> &InPtInt, vector<float> &FnPtInt);
};
