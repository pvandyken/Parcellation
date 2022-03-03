#pragma once
#include "../src/intersection.h"
#include "../src/orientation.h"
#include "../src/io.h"
#include "../src/mesh.h"
#include <Eigen/Dense>
#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>

using namespace Eigen;
namespace py = pybind11;

void orientBundles(vector<Bundle> &bundles);

int cortical_intersections(py::EigenDRef<const MatrixX3f> vertices,
                           py::EigenDRef<const MatrixX3i> polygons,
                           string bundle_dir,
                           string out,
                           const int nPtsLine);