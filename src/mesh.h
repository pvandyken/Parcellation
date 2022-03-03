#pragma once
#include <pybind11/eigen.h>
#include <Eigen/Dense>

namespace py = pybind11;
using namespace Eigen;

class Mesh {
public:
    Mesh(
        const py::EigenDRef<const MatrixX3f> &vertex,
        const py::EigenDRef<const MatrixX3i> &polygons
    ) : vertex{vertex}, polygons{polygons} {};

    const py::EigenDRef<const MatrixX3f> &vertex;
    const py::EigenDRef<const MatrixX3i> &polygons;
};