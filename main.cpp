/* #include "felipeTools.h" */
#include "intersection.h"
#include "ndarray.h"
#include <array>
#include <iostream>

using namespace std;


int main() {
  // float vertices[] = {
  //     0.,   0.,   0.,
  //     50.,   0.,   0.,
  //     0.,  50.,  0.,

  // };
  // int triangles[] = {0, 1, 2};

  // Ndarray<float> v_arr(vertices, {3,3}, {3,1});
  // Ndarray<int> t_arr(triangles, {1,3}, {3,1});

  // vector<vector<vector<vector<float>>>> bundles{
  //   {
  //       {{1,1,10}, {1,1,60}, {2,2,2}},
  //       {{3, 3, 10}, {3,3,60}, {4,4,2}}
  //   }
  // };

  // vector<vector<int>> InTri, FnTri, fib_index;
  // vector<vector<vector<float>>> InPoints, FnPoints;

  // const int ptsline = 2;
  // meshAndBundlesIntersection(v_arr, t_arr, bundles, ptsline, InTri,
  //                            FnTri, InPoints, FnPoints, fib_index);

  // for (auto tri : FnPoints) {
  //   for (auto x : tri) {
  //     for (auto coord : x) {
  //       cout << coord << " ";
  //     }
  //     cout << "\n";
  //   }
  //   cout << "\n" << endl;
  // }
  float data[] = {
      0.,   1.,   2.,   3.,   4.,   5.,   6.,
       7.,   8.,   9.,  10.,  11.,  12.,  13.,
      14.,  15.,  16.,  17.,  18.,  19.,  20.,

      21.,  22.,  23.,  24.,  25.,  26.,  27.,
      28.,  29.,  30.,  31.,  32.,  33.,  34.,
      35.,  36.,  37.,  38.,  39.,  40.,  41.,

      42.,  43.,  44.,  45.,  46.,  47.,  48.,
      49.,  50.,  51.,  52.,  53.,  54.,  55.,
      56.,  57.,  58.,  59.,  60.,  61.,  62.,

      63.,  64.,  65.,  66.,  67.,  68.,  69.,
      70.,  71.,  72.,  73.,  74.,  75.,  76.,
      77.,  78.,  79.,  80.,  81.,  82.,  83.,

      84.,  85.,  86.,  87.,  88.,  89.,  90.,
      91.,  92.,  93.,  94.,  95.,  96.,  97.,
      98.,  99., 100., 101., 102., 103., 104.,

     105., 106., 107., 108., 109., 110., 111.,
     112., 113., 114., 115., 116., 117., 118.,
     119., 120., 121., 122., 123., 124., 125.
  };
  Ndarray<float> test(data, {6,3,7}, {21,7,1});
  test.print();
}

// int cortical_intersections(Ndarray<float> Lvertex, Ndarray<float> Rvertex,
//                            Ndarray<int> Lpolygons, Ndarray<int> Rpolygons) {
//   int n_Lvertex, n_Lpolygons;
//   int n_Rvertex, n_Rpolygons;

//   // ================Se lee el mallado=======================


//   // ================ Se leen la fibras==============
//   int nLBundles, nRBundles;
//   vector<int> nLFibers, nRFibers;
//   vector<vector<int>> nLPoints, nRPoints;
//   vector<vector<vector<vector<float>>>> LPoints, RPoints;

//   // read_bundles(argv[3], nLBundles, nLFibers, nLPoints, LPoints);
//   // read_bundles(argv[4], nRBundles, nRFibers, nRPoints, RPoints);

//   // Intersección==========================
//   const int nPtsLine = 2;

//   vector<vector<int>> Lfib_index, Rfib_index;
//   vector<vector<int>> LInTri, LFnTri, RInTri, RFnTri;
//   vector<vector<vector<float>>> LInPoints, LFnPoints, RInPoints, RFnPoints;
//   vector<vector<vector<vector<float>>>> bundles{
//     {
//         {{1,1,10}, {1,1,60}, {2,2,2}},
//         {{3, 3, 10}, {3,3,60}, {4,4,2}}
//     }
//   };
//   meshAndBundlesIntersection(Lvertex, n_Lvertex, Lpolygons, n_Lpolygons,
//                              nLBundles, nLFibers, nLPoints, LPoints, nPtsLine,
//                              LInTri, LFnTri, LInPoints, LFnPoints, Lfib_index);

//   // meshAndBundlesIntersection(Rvertex, n_Rvertex, Rpolygons, n_Rpolygons,
//   //                            nRBundles, nRFibers, nRPoints, RPoints, nPtsLine,
//   //                            RInTri, RFnTri, RInPoints, RFnPoints, Rfib_index);

//   write_intersection(argv[5], argv[3], LInTri, LFnTri, LInPoints, LFnPoints,
//                      Lfib_index);
//   write_intersection(argv[6], argv[4], RInTri, RFnTri, RInPoints, RFnPoints,
//                      Rfib_index);

//   return 0;
// }
