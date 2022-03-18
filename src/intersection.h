#pragma once

#include <Eigen/Dense>
#include <functional>
#include <map>
#include <unordered_set>
#include <string>
#include <vector>
#include <pybind11/pybind11.h>

#include "bundleIntersection.h"
#include "bundles.h"
#include "orientation.h"
#include "mesh.h"
#include "types.h"
#include "utils.h"

using namespace std;
using namespace Eigen;
namespace py = pybind11;

class CorticalIntersection {
 public:
  CorticalIntersection() : mesh{Mesh()} {};
  CorticalIntersection(const Mesh &mesh, vector<Bundle> bundles,
                       const int &nPtsLine);

  CorticalIntersection(const Mesh &mesh,
                       vector<BundleIntersections> &interceptions)
      : mesh{mesh}, front{interceptions} {};

  const Mesh &mesh;
  vector<BundleIntersections> front;
  vector<BundleIntersections> back;
  vector<vector<int>> fibIndex;

  const vector<map<int, int>> &getTrianglesIntersected(
      fn<void> const &sigintHander = []() {});

  vector<const vector<int> *> getTrianglesFront();
  vector<const vector<int> *> getTrianglesBack();

  py::object getGlobbedGraph(int radius = 2);

  static CorticalIntersection fromBundles(
    Mesh const& mesh, string bundle_dir,
    const int nPtsLine, bool alignOrientation = false);

 private:
  vector<map<int, int>> trianglesIntersected;

  vector<const vector<int> *> getTriangles(
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
