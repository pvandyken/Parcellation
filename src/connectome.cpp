#include "connectome.h"

#include <algorithm>
#include <cppitertools/count.hpp>
#include <cppitertools/zip.hpp>
#include <utility>

#include "intersection.h"
#include "parcellation.h"

template <class T, class S>
using umap = std::unordered_map<T, S>;

Connectome::Connectome(CorticalIntersection const& intersections,
                       Parcellation const& parcellation)
    : matrix{Eigen::MatrixXi::Zero(parcellation.size(), parcellation.size())},
      bundleComp{{}}, lostFibers{0} {
  for (auto&& [bundleId, front, back] :
       iter::zip(iter::count(), intersections.front, intersections.back)) {
    for (auto&& [fTri, bTri] : iter::zip(front.triangles, back.triangles)) {
      if (parcellation.contains(fTri) && parcellation.contains(bTri)) {
        const int fParcel = parcellation.getTriangleMembership(fTri);
        const int bParcel = parcellation.getTriangleMembership(bTri);
        matrix(fParcel, bParcel) += 1;
        matrix(bParcel, fParcel) += 1;
        std::pair<int, int> edge{std::min(fParcel, bParcel),
                                 std::max(fParcel, bParcel)};
        // Add a record of the bundleid
        this->bundleComp[this->bcKey(edge)][bundleId]++;
      } else {
        this->lostFibers++;
      }
    }
  }
}

const umap<int, int> Connectome::getBundlesOfEdge(
    const std::pair<int, int>& edge) const{
  std::pair<int, int> oEdge{std::min(edge.first, edge.second),
                            std::max(edge.first, edge.second)};
  if (this->bundleComp.find(this->bcKey(oEdge)) ==
      this->bundleComp.end()) {
    throw std::out_of_range("Edge (" + std::to_string(edge.first) + ", " +
                            std::to_string(edge.second) +
                            ") not found in connectome");
  }
  return this->bundleComp.at(this->bcKey(oEdge));
  // int total;
  // for (auto& [bundle, count] : this->bundleComp[this->bcKey(oEdge)]) {
  //   total += count;
  // }
  // umap<int, float> normed;
  // for (auto& [bundle, count] : this->bundleComp[this->bcKey(oEdge)]) {
  //   normed[bundle] = count;// / total;
  // }
  // return normed;
}
