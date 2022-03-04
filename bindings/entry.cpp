#include "../src/intersection.h"
#include "../src/orientation.h"
#include "../src/io.h"
#include "../src/mesh.h"
#include <Eigen/Dense>
#include <tuple>
#include <vector>
#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/stl.h>
#include <Python.h>

using namespace Eigen;
namespace py = pybind11;

void sigintHandler()
{
  if (PyErr_CheckSignals() != 0)
    throw py::error_already_set();
}

void orientBundles(vector<Bundle> &bundles)
{
  for (auto &bundle : bundles)
  {
    Orientation::alignOrientation(bundle.fibers);
  }
}

void getDensityGroups(
    vector<BundleIntersections> &intersections,
    vector<map<int, int>> const& trianglesIntersected,
    Mesh &mesh,
    vector<vector<int>> &result)
{
  for (auto & intersection : intersections)
  {
    map<int, float> const& probabilities = intersection.getTriangleProbabilities(
        mesh, trianglesIntersected, sigintHandler);
    result.push_back(intersection.getIntersectionCore(0.15, probabilities));
  }
}


tuple<vector<vector<int>>, vector<vector<int>>>
intersectionCore(const py::EigenDRef<const MatrixX3f> vertices,
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

  tuple<vector<vector<int>>, vector<vector<int>>> densityGroups {vector<vector<int>>(), vector<vector<int>>()};
  auto trianglesIntersected = intersections.getTrianglesIntersected(sigintHandler);
  getDensityGroups(intersections.front, trianglesIntersected, mesh, get<0>(densityGroups));
  getDensityGroups(intersections.back, trianglesIntersected, mesh, get<1>(densityGroups));

  return densityGroups;
}

PYBIND11_MODULE(intersection, m)
{
  m.doc() = "Module for finding the intersection of white matter bundles with the cortex";
  m.def("intersection_core", &intersectionCore, "Find the intersections!");
}