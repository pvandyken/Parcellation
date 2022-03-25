#ifndef INTER_PARCELLATION_H
#define INTER_PARCELLATION_H

#include <unordered_map>
#include <vector>
#include <unordered_set>

#include <Eigen/Dense>

#include "intersection.h"

class Parcellation {
 public:
  Parcellation(const std::vector<std::unordered_set<int>> parcels)
    : parcels{parcels} {};

  const int getTriangleMembership(const int triangle) const;
  const bool contains(const int triangle) const;
  const size_t size() const{
    return this->parcels.size();
  }

  std::vector<std::unordered_set<int>>::const_iterator begin();
  std::vector<std::unordered_set<int>>::const_iterator end();

 private:
  const std::vector<std::unordered_set<int>> parcels;
  mutable std::unordered_map<int, int> triangleMembership;
  const void initTriangleMembership() const;
  const bool _contains(const int triangle) const;
};

#endif
