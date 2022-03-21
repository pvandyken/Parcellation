#include "parcellation.h"

#include <cppitertools/enumerate.hpp>
#include <cppitertools/zip.hpp>

int Parcellation::getTriangleMembership(const int triangle) {
  if (this->triangleMembership.size() == 0) {
    for (auto&& [i, parcel] : iter::enumerate(this->parcels)) {
      for (auto triangle : parcel) {
        if (this->triangleMembership.find(triangle) !=
            this->triangleMembership.end()) {
          throw std::invalid_argument(
              "Each triangle may only be a member of one segment. " +
              std::to_string(triangle) + " was found twice.");
        }
        this->triangleMembership[triangle] = i;
      }
    }
  }
  return this->triangleMembership[triangle];
}

Eigen::MatrixXi Parcellation::getConnectome(
    CorticalIntersection const& intersections) {
  Eigen::MatrixXi result =
      Eigen::MatrixXi::Zero(this->parcels.size(), this->parcels.size());
  for (auto&& [front, back] :
       iter::zip(intersections.front, intersections.back)) {
    for (auto&& [fTri, bTri] : iter::zip(front.triangles, back.triangles)) {
      int fParcel = this->getTriangleMembership(fTri);
      int bParcel = this->getTriangleMembership(bTri);
      result(fParcel, bParcel) += 1;
    }
  }
  return result;
}