#include <Python.h>
#include <omp.h>
#include <pybind11/eigen.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl/filesystem.h>

#include <Eigen/Dense>
#include <filesystem>
#include <tuple>
#include <vector>

#include "../src/bundles.h"
#include "../src/connectome.h"
#include "../src/intersection.h"
#include "../src/io.h"
#include "../src/mesh.h"
#include "../src/orientation.h"
#include "../src/parcellation.h"

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

// tuple<vector<vector<int>>, vector<vector<int>>> intersectionCore(
//     const py::EigenDRef<const MatrixX3f> vertices,
//     const py::EigenDRef<const MatrixX3i> polygons, string bundle_dir,
//     // string out,
//     const int nPtsLine, const float probThreshold) {
//   // omp_set_num_threads(4);
//   vector<Bundle> bundles;

//   IO::read_bundles(bundle_dir, bundles);
//   orientBundles(bundles);

//   Mesh mesh(vertices, polygons);

//   CorticalIntersection intersections(mesh, bundles, nPtsLine);

//   // IO::write_intersection(out, bundle_dir,
//   //                        intersections.front, intersections.back,
//   //                        intersections.fibIndex);

//   tuple<vector<vector<int>>, vector<vector<int>>> densityGroups{
//       vector<vector<int>>(), vector<vector<int>>()};
//   auto trianglesIntersected =
//       intersections.getTrianglesIntersected(sigintHandler);
//   getDensityGroups(intersections.front, trianglesIntersected, mesh,
//                    get<0>(densityGroups));
//   getDensityGroups(intersections.back, trianglesIntersected, mesh,
//                    get<1>(densityGroups));
//   return densityGroups;
// }

PYBIND11_MODULE(cortical_intersections, m) {
  m.doc() =
      "Module for finding the intersection of white matter bundles with the "
      "cortex";
  // m.def("intersection_core", &intersectionCore, "Find the intersections!");

  py::class_<Mesh>(m, "Mesh")
      .def(py::init<const string &>(), py::arg("name"))
      .def(py::init<const std::filesystem::path>(), py::arg("name"))
      .def_property_readonly("polydata", &Mesh::getPolydata,
                             "Mesh as a vtk Polydata object")
      .def_readonly("vertices", &Mesh::vertices)
      .def_readonly("polygons", &Mesh::polygons)
      .def("filter_triangles", &Mesh::filterTriangles,
           "Get a subset of the original mesh", py::arg("whitelist"))
      .def("save_vtk", &Mesh::saveVtk, "Save mesh as .vtk file",
           py::arg("path"), py::arg("binary") = false)
      .def("get_triangle_neighbors",
           py::overload_cast<int, bool>(&Mesh::getTriangleNeighbors),
           "Get triangle neighbors", py::arg("triangle"),
           py::arg("edge_adjacent") = false)
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
           "Get triangles adjacent to a specific point")
      .def(py::pickle(
          [](Mesh &self) {
            return py::make_tuple(self.vertices, self.polygons);
          },
          [](py::tuple t) {
            if (t.size() != 2) throw std::runtime_error("Invalid state!");
            Mesh m(t[0].cast<Eigen::MatrixX3f>(),
                   t[1].cast<Eigen::MatrixX3i>());
            return m;
          }));

  py::class_<CorticalIntersection>(m, "CorticalIntersection")
      .def(py::init<Mesh &, vector<Bundle>, int, int>(), py::arg("mesh"),
           py::arg("bundles"), py::arg("ray_length"), py::arg("threads") = 1,
           py::keep_alive<1, 2>())
      .def_readonly("bundles", &CorticalIntersection::bundles)
      .def_property_readonly(
          "mesh", [](CorticalIntersection &self) { return self.mesh; })
      .def_static("from_bundles", &CorticalIntersection::fromBundles,
                  "Construct intersections from bundles")
      .def_property_readonly("triangles",
                             [](CorticalIntersection &self) {
                               return py::make_tuple(self.getTrianglesFront(),
                                                     self.getTrianglesBack());
                             })
      .def("get_globbed_graph", &CorticalIntersection::getGlobbedGraph,
           "Generate a networkx directed graph containing "
           "each interection as a node",
           py::arg("radius") = 2)
      .def("get_bundle_path", &CorticalIntersection::getBundlePath);

  py::class_<Bundle>(m, "Bundle")
      .def(py::init<vector<vector<Vector3f>>>(),
           "Bundles of white-matter fibers")
      .def_readonly("path", &Bundle::path)
      .def(py::init<vector<vector<Vector3f>>, const string>(),
           "Bundles of white-matter fibers");

  py::class_<Parcellation>(m, "Parcellation")
      .def(py::init<vector<unordered_set<int>>>(), "Get connectomes")
      .def("get_triangle_membership", &Parcellation::getTriangleMembership)
      .def("__contains__", &Parcellation::contains)
      .def("__len__", &Parcellation::size)
      .def(
          "__iter__",
          [](Parcellation &self) {
            return py::make_iterator(self.begin(), self.end());
          },
          py::keep_alive<0, 1>());

  py::class_<Connectome>(m, "Connectome")
      .def(py::init<CorticalIntersection const &, Parcellation const &>(),
           py::arg("intersection"), py::arg("parcellation"))
      .def("get_bundles_of_edge", &Connectome::getBundlesOfEdge)
      .def_property_readonly("lost_fibers", &Connectome::getLostFibers)
      .def_readonly("matrix", &Connectome::matrix)
      .def(py::pickle(
          [](const Connectome &self) {  // __getstate__
            return py::make_tuple(self.matrix, self.getBundleComp(),
                                  self.getLostFibers());
          },
          [](py::tuple t) {  // __setstate__
            if (t.size() != 3) throw std::runtime_error("Invalid state!");

            Connectome c(t[0].cast<Eigen::MatrixXi>(),
                         t[1].cast<Connectome::bundleCompType>(),
                         t[2].cast<int>());
            return c;
          }));
}