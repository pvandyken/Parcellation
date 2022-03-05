#pragma once

#include <iostream>
#include <map>
#include <vector>

#include "mesh.h"
#include "types.h"

using namespace std;

class BundleIntersections {
 public:
  BundleIntersections()
      : triangles{vector<int>()}, points{vector<vector<float>>()} {};
  BundleIntersections(const size_t id) : id{id} {};
  BundleIntersections(vector<int> const &triangles,
                      vector<vector<float>> const &points)
      : triangles{triangles}, points{points} {};

  size_t id;
  vector<int> triangles;
  vector<vector<float>> points;

  map<int, float> getTriangleProbabilities(
      Mesh &mesh, vector<map<int, int>> const &triangleIntersections,
      fn<void> const &sigintHandler = []() {});
  static vector<int> getIntersectionCore(float const &threshold,
                                         map<int, float> const &probabilities);

  static vector<BundleIntersections> fromRange(size_t size);
};