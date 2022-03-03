#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "src/mesh.h"
#include "src/intersection.h"
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
               10, 4, 5, // 4
               7, 8, 6, // 5
               9, 0, 5; // 6

  vector<vector<int>> interceptions {
    {0, 0, 1, 1, 0, 2, 0, 1, 0},
    {2, 2, 1, 0, 2, 0, 3, 1},
    {5, 2, 1, 6, 2, 0, 3},
    {1, 2, 1, 1, 4, 0, 1, 2, 1},
    {6, 5, 4, 5, 5, 6, 4, 3, 5}
  };

  Mesh mesh(points, triangles);

  CorticalIntersection intersection(mesh, interceptions);

  REQUIRE( mesh.getTrianglesOfPoint(0) == vector<int> {0, 3, 6});
  REQUIRE( mesh.getTrianglesOfPoint(3) == vector<int> {1, 2, 3});

  REQUIRE( mesh.getTriangleNeighbors(1) == vector<int> {0, 1, 2, 3});
  REQUIRE( mesh.getTriangleNeighbors(5) == vector<int> {2, 5});

  REQUIRE( intersection.getTrianglesIntersected(0) == map<int, int>{ {0, 5}, {1, 2}, {2, 1}, {3, 1}});
  REQUIRE( intersection.getTrianglesIntersected(2) == map<int, int>{ {0, 1}, {1, 3}, {2, 2}, {3, 2}});
  REQUIRE( intersection.getTrianglesIntersected(6) == map<int, int>{ {2, 1}, {4, 2}});
}
