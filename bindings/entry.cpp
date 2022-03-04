#include "../src/intersection.h"
#include "../src/orientation.h"
#include "../src/io.h"
#include "../src/mesh.h"
#include <Eigen/Dense>
#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/stl.h>
#include <Python.h>

using namespace Eigen;
namespace py = pybind11;


void orientBundles(vector<Bundle> &bundles)
{
  for (auto &bundle : bundles)
  {
    Orientation::alignOrientation(bundle.fibers);
  }
}

void sigintHandler() {
  if (PyErr_CheckSignals() != 0)
    throw py::error_already_set();
}
vector<vector<int>> intersectionCore(const py::EigenDRef<const MatrixX3f> vertices,
                           const py::EigenDRef<const MatrixX3i> polygons,
                           string bundle_dir,
                           string out,
                           const int nPtsLine,
                           const float probThreshold)
{
  vector<Bundle> bundles;

  IO::read_bundles(bundle_dir, bundles);
  orientBundles(bundles);

  Mesh mesh(vertices, polygons);

  CorticalIntersection intersections(mesh, bundles, nPtsLine);

  IO::write_intersection(out, bundle_dir,
                         intersections.front, intersections.back,
                         intersections.fibIndex);

  vector<vector<int>> frontDensityGroups;
  cout << "Calculate intersections per triangle" << endl;
  vector<map<int, int>> const& trianglesIntersected = intersections.getTrianglesIntersected(sigintHandler);
  cout << "Getting triangle intersection probabilities..." << endl;
  for (auto intersection : intersections.front) {
    cout << intersection.id << " " << endl;
    map<int, float> probabilities = intersection.getTriangleProbabilities(
      mesh, trianglesIntersected, sigintHandler
    );
    frontDensityGroups.push_back(intersection.getIntersectionCore(0.15, probabilities));
  }
  cout << endl;
  cout << "Returning answer" << endl;
  return frontDensityGroups;
}

PYBIND11_MODULE(intersection, m) {
  m.doc() = "Module for finding the intersection of white matter bundles with the cortex";
  m.def("intersection_core", &intersectionCore, "Find the intersections!");
}