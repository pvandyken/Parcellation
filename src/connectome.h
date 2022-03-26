#pragma once

#include <Eigen/Dense>
#include <unordered_map>
#include <utility>

#include "intersection.h"
#include "parcellation.h"

class Connectome {
 public:
  using bundleCompType =
      std::unordered_map<uint64_t, std::unordered_map<int, int>>;

  Connectome(const CorticalIntersection &intersections,
             const Parcellation &parcellation);
  Connectome(Eigen::MatrixXi matrix, bundleCompType bundleComp, int lostFibers)
      : matrix{matrix}, bundleComp{bundleComp}, lostFibers{lostFibers} {};

  Eigen::MatrixXi matrix;

  const std::unordered_map<int, int> getBundlesOfEdge(
      const std::pair<int, int> &edge) const;
  const int getLostFibers() const { return this->lostFibers; }
  const bundleCompType getBundleComp() const { return this->bundleComp; }

 private:
  const uint64_t bcKey(const std::pair<int, int> key) const {
    return (uint64_t)key.first << 32 | (uint64_t)key.second;
  }
  bundleCompType bundleComp;
  int lostFibers;
};
