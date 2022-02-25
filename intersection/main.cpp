#include "intersection.h"
#include "felipeTools.h"

using namespace std;

int main(int argc, char const *argv[])
{
	vector<vector<float>> Lvertex, Rvertex;
	vector<vector<int>> Lpolygons, Rpolygons;
	int n_Lvertex, n_Lpolygons;
	int n_Rvertex, n_Rpolygons;

	// ========================== Se lee el mallado =======================================

	read_mesh(argv[1], Lvertex, Lpolygons, n_Lvertex, n_Lpolygons);
	read_mesh(argv[2], Rvertex, Rpolygons, n_Rvertex, n_Rpolygons);

	// ========================== Se leen las fibras ======================================
	int nLBundles, nRBundles;
	vector<int> nLFibers, nRFibers;
	vector<vector<int>> nLPoints, nRPoints;
	vector<vector<vector<vector<float>>>> LPoints, RPoints;

	read_bundles(argv[3], nLBundles, nLFibers, nLPoints, LPoints);
	read_bundles(argv[4], nRBundles, nRFibers, nRPoints, RPoints);

	// =========================== Intersección ===========================================
	const int nPtsLine = 2;

	vector<vector<int>> Lfib_index, Rfib_index;
	vector<vector<int>> LInTri, LFnTri, RInTri, RFnTri;
	vector<vector<vector<float>>> LInPoints, LFnPoints, RInPoints, RFnPoints;

	meshAndBundlesIntersection(Lvertex, n_Lvertex, Lpolygons, n_Lpolygons, nLBundles, nLFibers,
							   nLPoints, LPoints, nPtsLine, LInTri, LFnTri, LInPoints, LFnPoints, Lfib_index);

	meshAndBundlesIntersection(Rvertex, n_Rvertex, Rpolygons, n_Rpolygons, nRBundles, nRFibers,
							   nRPoints, RPoints, nPtsLine, RInTri, RFnTri, RInPoints, RFnPoints, Rfib_index);

	write_intersection(argv[5], argv[3], LInTri, LFnTri, LInPoints, LFnPoints, Lfib_index);
	write_intersection(argv[6], argv[4], RInTri, RFnTri, RInPoints, RFnPoints, Rfib_index);


	return 0;
}
