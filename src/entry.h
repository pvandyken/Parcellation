#pragma once
#include "ndarray.h"
#include "intersection.h"
#include "io.h"

int test_func(string var) {
  return 1;
}

int cortical_intersections(Ndarray<float> vertices,
                           Ndarray<int> polygons,
                           string bundle_dir,
                           string out) {


  int nLBundles, nRBundles;
  vector<int> nFibers;
  vector<vector<int>> nPoints;
  vector<vector<vector<vector<float>>>> LPoints;

  read_bundles(bundle_dir, nLBundles, nFibers, nPoints, LPoints);

  const int nPtsLine = 2;

  vector<vector<int>> Lfib_index, Rfib_index;
  vector<vector<int>> InitTriangles, FinalTriangles;
  vector<vector<vector<float>>> InPoints, FnPoints;
  vector<vector<vector<vector<float>>>> bundles{
    {
        {{1,1,10}, {1,1,60}, {2,2,2}},
        {{3, 3, 10}, {3,3,60}, {4,4,2}}
    }
  };
  meshAndBundlesIntersection(vertices, polygons,
                             bundles, nPtsLine,
                             InitTriangles, FinalTriangles, InPoints, FnPoints, Lfib_index);


  write_intersection(out, bundle_dir, InitTriangles, FinalTriangles, InPoints, FnPoints,
                     Lfib_index);

  return 0;
}