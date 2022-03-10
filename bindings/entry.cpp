#include <Python.h>
#include <omp.h>
#include <pybind11/eigen.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <Eigen/Dense>
#include <tuple>
#include <vector>

#include "../src/intersection.h"
#include "../src/io.h"
#include "../src/mesh.h"
#include "../src/orientation.h"

using namespace Eigen;
namespace py = pybind11;

void sigintHandler() {
  if (PyErr_CheckSignals() != 0) throw py::error_already_set();
}

void orientBundles(vector<Bundle> &bundles) {
  for (auto &bundle : bundles) {
    Orientation::alignOrientation(bundle.fibers);
  }
}

void getDensityGroups(vector<BundleIntersections> &intersections,
                      vector<map<int, int>> const &trianglesIntersected,
                      Mesh &mesh, vector<vector<int>> &result) {
  for (auto &intersection : intersections) {
    map<int, float> const &probabilities =
        intersection.getTriangleProbabilities(mesh, trianglesIntersected,
                                              sigintHandler);
    result.push_back(intersection.getIntersectionCore(0.15, probabilities));
  }
}

tuple<vector<vector<int>>, vector<vector<int>>> intersectionCore(
    const py::EigenDRef<const MatrixX3f> vertices,
    const py::EigenDRef<const MatrixX3i> polygons, string bundle_dir,
    // string out,
    const int nPtsLine, const float probThreshold) {
  // omp_set_num_threads(4);
  vector<Bundle> bundles;

  IO::read_bundles(bundle_dir, bundles);
  orientBundles(bundles);

  Mesh mesh(vertices, polygons);

  CorticalIntersection intersections(mesh, bundles, nPtsLine);

  // IO::write_intersection(out, bundle_dir,
  //                        intersections.front, intersections.back,
  //                        intersections.fibIndex);

  tuple<vector<vector<int>>, vector<vector<int>>> densityGroups{
      vector<vector<int>>(), vector<vector<int>>()};
  auto trianglesIntersected =
      intersections.getTrianglesIntersected(sigintHandler);
  getDensityGroups(intersections.front, trianglesIntersected, mesh,
                   get<0>(densityGroups));
  getDensityGroups(intersections.back, trianglesIntersected, mesh,
                   get<1>(densityGroups));
  return densityGroups;
}

PYBIND11_MODULE(cortical_intersections, m) {
  m.doc() =
      "Module for finding the intersection of white matter bundles with the "
      "cortex";
  m.def("intersection_core", &intersectionCore, "Find the intersections!");

  py::class_<Mesh>(m, "MeshData")
      .def(py::init<EigenDRef<const MatrixX3f>, EigenDRef<const MatrixX3i>>())
      .def_readonly("vertices", &Mesh::vertices)
      .def_readonly("polygons", &Mesh::polygons)
      .def("get_triangle_neighbors",
           py::overload_cast<int>(&Mesh::getTriangleNeighbors),
           "Get triangle neighbors")
      .def("get_triangle_neighbors",
           py::overload_cast<vector<int>>(&Mesh::getTriangleNeighbors),
           "Get triangle neighbors")
      .def("get_triangle_neighbors",
           py::overload_cast<vector<int>, vector<int>>(
               &Mesh::getTriangleNeighbors),
           "Get triangle neighbors")
      .def("get_triangles_of_point",
           py::overload_cast<int>(&Mesh::getTrianglesOfPoint),
           "Get triangles adjacent to a specific point")
      .def("get_triangles_of_point",
           py::overload_cast<vector<int>>(&Mesh::getTrianglesOfPoint),
           "Get triangles adjacent to a specific point");

  py::class_<CorticalIntersection>(m, "CorticalIntersection")
      .def(py::init<>())
      .def_static("from_bundles", &CorticalIntersection::fromBundles,
                  "Construct intersections from bundles")
      .def("get_triangles_front", &CorticalIntersection::getTrianglesFront,
           "Get Intersected Triangles")
      .def("get_triangles_back", &CorticalIntersection::getTrianglesBack,
           "Get Intersected Triangles");
}