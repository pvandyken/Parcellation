#include "parcellation.h"

#include <cppitertools/enumerate.hpp>
#include <cppitertools/zip.hpp>

std::vector<std::unordered_set<int>>::const_iterator Parcellation::begin() {
  return this->parcels.begin();
}

std::vector<std::unordered_set<int>>::const_iterator Parcellation::end() {
  return this->parcels.end();
}

int Parcellation::getTriangleMembership(const int triangle) {
  if (this->triangleMembership.size() == 0) {
    this->initTriangleMembership();
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
      if (this->contains(fTri) && this->contains(bTri)) {
        int fParcel = this->getTriangleMembership(fTri);
        int bParcel = this->getTriangleMembership(bTri);
        result(fParcel, bParcel) += 1;
        result(bParcel, fParcel) += 1;
      }
    }
  }
  return result;
}

bool Parcellation::contains(const int triangle) {
  if (this->triangleMembership.size() == 0) {
    this->initTriangleMembership();
  }
  return this->_contains(triangle);
}

bool Parcellation::_contains(const int triangle) {
  if (this->triangleMembership.find(triangle) !=
      this->triangleMembership.end()) {
    return true;
  }
  return false;
}

void Parcellation::initTriangleMembership() {
  for (auto&& [i, parcel] : iter::enumerate(this->parcels)) {
    for (auto triangle : parcel) {
      if (this->_contains(triangle)) {
        throw std::invalid_argument(
            "Each triangle may only be a member of one segment. " +
            std::to_string(triangle) + " was found twice.");
      }
      this->triangleMembership[triangle] = i;
    }
  }
}