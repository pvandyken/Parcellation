#include "bundleIntersection.h"

map<int, float> BundleIntersections::getTriangleProbabilities(
    Mesh &mesh, vector<map<int, int>> const &triangleIntersections,
    fn<void> const &sigintHandler) {
  map<int, float> probabilities;
  cout << "Getting core of bundle " << this->id << endl;
  cout << this->triangles.size() << " triangles" << endl;
  for (auto const &triangle : this->triangles) {
    sigintHandler();
    const vector<int> neighbors = mesh.getTriangleNeighbors(triangle);
    cout.flush();
    uint16_t grandTotal = 0;
    uint16_t bundleGrandTotal = 0;
    for (auto const &neighbor : neighbors) {
      // Get total number of times each neighbor is intersected
      const map<int, int> &neighborIntersections =
          triangleIntersections[neighbor];
      uint16_t total = 0;
      for (auto const &[bundle, count] : neighborIntersections) {
        total += count;
      }

      // Get number of times neighbor is intersected by bundle
      const int &bundleTotal =
          (neighborIntersections.find(this->id) != neighborIntersections.end())
              ? neighborIntersections.at(this->id)
              : 0;

      // Add to grand totals
      grandTotal += total;
      bundleGrandTotal += bundleTotal;
    }
    probabilities.insert({triangle, (float)bundleGrandTotal / grandTotal});
  }
  return probabilities;
}

vector<BundleIntersections> BundleIntersections::fromRange(size_t size) {
  vector<BundleIntersections> result;
  result.reserve(size);
  for (size_t i = 0; i < size; i++) {
    result.push_back(BundleIntersections(i));
  }
  return result;
}

vector<int> BundleIntersections::getIntersectionCore(
    float const &threshold, map<int, float> const &probabilities) {
  vector<int> result;
  for (auto const &[key, value] : probabilities) {
    if (value >= threshold) {
      result.push_back(key);
    }
  }
  return result;
}