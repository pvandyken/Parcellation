#include "parcellation.h"

#include <cppitertools/count.hpp>
#include <cppitertools/enumerate.hpp>
#include <cppitertools/zip.hpp>

std::vector<std::unordered_set<int>>::const_iterator Parcellation::begin() {
  return this->parcels.begin();
}

std::vector<std::unordered_set<int>>::const_iterator Parcellation::end() {
  return this->parcels.end();
}

const int Parcellation::getTriangleMembership(const int triangle) const{
  if (this->triangleMembership.size() == 0) {
    this->initTriangleMembership();
  }
  return this->triangleMembership[triangle];
}


const bool Parcellation::contains(const int triangle) const{
  if (this->triangleMembership.size() == 0) {
    this->initTriangleMembership();
  }
  return this->_contains(triangle);
}

const bool Parcellation::_contains(const int triangle) const{
  if (this->triangleMembership.find(triangle) !=
      this->triangleMembership.end()) {
    return true;
  }
  return false;
}

const void Parcellation::initTriangleMembership() const{
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