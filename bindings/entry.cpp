#include "entry.h"

void orientBundles(vector<Bundle> &bundles)
{
  for (auto &bundle : bundles)
  {
    Orientation::alignOrientation(bundle.fibers);
  }
}

int cortical_intersections(const py::EigenDRef<const MatrixX3f> vertices,
                           const py::EigenDRef<const MatrixX3i> polygons,
                           string bundle_dir,
                           string out,
                           const int nPtsLine)
{

  int nLBundles, nRBundles;
  vector<int> nFibers;
  vector<vector<int>> nPoints;
  vector<Bundle> bundles;

  IO::read_bundles(bundle_dir, bundles);
  orientBundles(bundles);

  Mesh mesh(vertices, polygons);

  cout << "Number of bundles: " << nLBundles << "\nNumber of fibres: "
       << nFibers[0] << endl;
  CorticalIntersection intersections(mesh, bundles, nPtsLine);

  IO::write_intersection(out, bundle_dir, intersections.inTri, intersections.fnTri, 
                         intersections.inPoints, intersections.fnPoints,
                         intersections.fibIndex);

  return 0;
}

PYBIND11_MODULE(intersection, m) {
  m.doc() = "Module for finding the intersection of white matter bundles with the cortex";
  m.def("cortical_intersections", &cortical_intersections, "Find the intersections!");
}