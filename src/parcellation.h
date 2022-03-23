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

  int getTriangleMembership(const int triangle);
  Eigen::MatrixXi getConnectome(CorticalIntersection const& intersections);
  bool contains(const int triangle);

  std::vector<std::unordered_set<int>>::const_iterator begin();
  std::vector<std::unordered_set<int>>::const_iterator end();

 private:
  const std::vector<std::unordered_set<int>> parcels;
  std::unordered_map<int, int> triangleMembership;
  void initTriangleMembership();
  bool _contains(const int triangle);
};

#endif
