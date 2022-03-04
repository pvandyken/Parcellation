#include "bundleIntersection.h"

map<int, float> BundleIntersections::getTriangleProbabilities(
    Mesh &mesh, const vector<map<int, int>> &triangleIntersections
) {
    map<int, float> probabilities;
    for (auto const& triangle : this->triangles) {
        const vector<int> neighbors = mesh.getTriangleNeighbors(triangle);
        uint16_t grandTotal = 0;
        uint16_t bundleGrandTotal = 0;
        for (auto const& neighbor : neighbors) {
            // Get total number of times each neighbor is intersected
            const map<int, int> &neighborIntersections = triangleIntersections[neighbor];
            uint16_t total = 0;
            for (auto const& [bundle, count] : neighborIntersections) {
                total += count;
            }

            // Get number of times neighbor is intersected by bundle
            const int &bundleTotal = (
                neighborIntersections.find(this->id) != neighborIntersections.end()
             ) ? neighborIntersections.at(this->id) : 0;

            // Add to grand totals
            grandTotal += total;
            bundleGrandTotal += bundleTotal;
        }
        probabilities.insert({triangle, (float)bundleGrandTotal/grandTotal});
    }
    return probabilities;
}


void BundleIntersections::fromRange(size_t size, vector<BundleIntersections> &arr) {
    arr.resize(size);
    for (size_t i = 0; i<size; i++) {
        arr.push_back(BundleIntersections(i));
    }
}

vector<int> BundleIntersections::getIntersectionCore(float const& threshold, map<int, float> const& probabilities) {
    vector<int> result;
    for (auto const& [key, value] : probabilities) {
        if (value >= threshold) {
            result.push_back(key);
        }
    }
    return result;
}