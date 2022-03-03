#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "src/mesh.h"
#include <array>
#include <iostream>
#include <vector>
#include <unordered_set>

using namespace std;


TEST_CASE( "Triangles formed by each point are calculated", "[mesh]") {
  MatrixX3f points(11, 3);
  // 11 Points
  points << 0, 0, 0, // 0
            1, 1, 1, // 1
            3, 3, 2, // 2
            4, 2, 3, // 3
            82352,134, 253, // 4
            15, 2352, 235, // 5
            25, 3253, 13, // 6
            262, 2351, 32, // 7
            523524, 24523, 1, // 8
            1612, 1423, 7523, // 9
            135, 6426, 152; // 10
  MatrixX3i triangles(7, 3);
  triangles << 0, 1, 2, // 0
               1, 2, 3, // 1
               3, 2, 6, // 2
               1, 0, 3, // 3
               10, 3, 2, // 4
               7, 8, 2, // 5
               9, 0, 5; // 6

  Mesh mesh(points, triangles);

  REQUIRE( mesh.getTrianglesOfPoint(0) == vector<int> {0, 3, 6});
  REQUIRE( mesh.getTrianglesOfPoint(3) == vector<int> {1, 2, 3, 4});
}
