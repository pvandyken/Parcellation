#pragma once

#include <Eigen/Dense>
#include <unordered_map>
#include <utility>

#include "intersection.h"
#include "parcellation.h"

class Connectome {
 public:
  Connectome(const CorticalIntersection &intersections,
             const Parcellation &parcellation);

  Eigen::MatrixXi matrix;

  const std::unordered_map<int, int> getBundlesOfEdge(
      const std::pair<int, int> &edge);
  const int getLostFibers() { return this->lostFibers; }

 private:
  const uint64_t bcKey(std::pair<int, int> key) {
    return (uint64_t)key.first << 32 | (uint64_t)key.second;
  }
  std::unordered_map<uint64_t, std::unordered_map<int, int>> bundleComp;
  int lostFibers;
};
