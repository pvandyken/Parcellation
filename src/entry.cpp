#include "entry.h"

void orientBundles(vector<vector<vector<Vector3f>>> &bundles)
{
  for (auto &bundle : bundles)
  {
    Orientation::alignOrientation(bundle);
  }
}

int cortical_intersections(py::EigenDRef<const MatrixX3f> vertices,
                           py::EigenDRef<const MatrixX3i> polygons,
                           string bundle_dir,
                           string out,
                           const int nPtsLine)
{

  int nLBundles, nRBundles;
  vector<int> nFibers;
  vector<vector<int>> nPoints;
  vector<vector<vector<Vector3f>>> bundles;

  IO::read_bundles(bundle_dir, nLBundles, nFibers, nPoints, bundles);
  orientBundles(bundles);
  cout << "Example fibre\nFront: " << bundles[0][0].front().transpose() << "\nBack: " << bundles[0][0].back().transpose() << endl;

  cout << "Number of bundles: " << nLBundles << "\nNumber of fibres: "
       << nFibers[0] << endl;
  vector<vector<int>> Lfib_index, Rfib_index;
  vector<vector<int>> InitTriangles, FinalTriangles;
  vector<vector<vector<float>>> InPoints, FnPoints;
  Intersection::meshAndBundlesIntersection(vertices, polygons,
                                           bundles, nPtsLine,
                                           InitTriangles, FinalTriangles, InPoints, FnPoints, Lfib_index);

  IO::write_intersection(out, bundle_dir, InitTriangles, FinalTriangles, InPoints, FnPoints,
                     Lfib_index);

  return 0;
}

PYBIND11_MODULE(intersection, m) {
  m.doc() = "Module for finding the intersection of white matter bundles with the cortex";
  m.def("cortical_intersections", &cortical_intersections, "Find the intersections!");
}