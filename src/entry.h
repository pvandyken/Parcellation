#pragma once
#include "intersection.h"
#include "orientation.h"
#include "io.h"
#include <Eigen/Dense>
#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>

using namespace Eigen;
namespace py = pybind11;

void orientBundles(vector<vector<vector<Vector3f>>> &bundles);

int cortical_intersections(py::EigenDRef<const MatrixX3f> vertices,
                           py::EigenDRef<const MatrixX3i> polygons,
                           string bundle_dir,
                           string out,
                           const int nPtsLine);