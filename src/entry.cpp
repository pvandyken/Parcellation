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

  cout << "Number of bundles: " << nLBundles << "\nNumber of fibres: "
       << nFibers[0] << endl;
  CorticalIntersection intersections(vertices, polygons,
                                           bundles, nPtsLine);

  IO::write_intersection(out, bundle_dir, intersections.inTri, intersections.fnTri, 
                         intersections.inPoints, intersections.fnPoints,
                         intersections.fibIndex);

  return 0;
}

PYBIND11_MODULE(intersection, m) {
  m.doc() = "Module for finding the intersection of white matter bundles with the cortex";
  m.def("cortical_intersections", &cortical_intersections, "Find the intersections!");
}