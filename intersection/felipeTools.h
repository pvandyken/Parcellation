#pragma once

#include "dirent.h"
#include <sys/stat.h>
#include <algorithm>
#include <fstream>
#include <string>
#include <vector>
#include "math.h"

using namespace std;

void read_mesh(const string &filename, vector<vector<float>> &vertex, vector<vector<int>> &polygons, int &len_vertex, int &len_polygons);
void read_bundles(const string &path, int &nBundles, vector<int> &nFibers, vector<vector<int>> &nPoints, vector<vector<vector<vector<float>>>> &Points) ;

void write_intersection(const string &path, const string &pathBundles,
	const vector<vector<int>> &InTri, const vector<vector<int>> &FnTri,
	const vector<vector<vector<float>>> &InPoints, const vector<vector<vector<float>>> &FnPoints,
	const vector<vector<int>> &fib_index);

void Delete(float **&vertex, const int &len_vertex, const int &len_polygons);