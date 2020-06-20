#include "felipeTools.h"
#include <iostream>

// función que lee el mallado (vértices y polígonos de los triángulos)
void read_mesh(const string &filename, vector<vector<float>>&vertex, vector<vector<int>> &polygons, int &len_vertex, int &len_polygons) {

	ifstream file(filename, ios::in | ios::binary);

	if(file.is_open()) {

		file.seekg(17);

		int dim;

		file.read ((char *) & dim, sizeof(int)); // dimensiones

		file.seekg(29);

		file.read ((char *) & len_vertex, sizeof(int)); // largo de vértices

		vector<vector<float>> vertex(len_vertex,vector<float>(3));

		for(int i=0; i<len_vertex; i++) {
			//vertex[i] = new float[3];

			for(int j=0; j<3; j++)
				file.read ((char *) & vertex[i][j], sizeof(float)); // lee cada punto (x,y,z)
		}

		file.seekg(4*3*len_vertex + 41);

		file.read ((char*)&len_polygons, sizeof(int)); // largo de polígonos
		vector<vector<int>> polygons(len_polygons,vector<int>(3));

		for(int i = 0; i < len_polygons; i++) {
			//polygons[i] = new int[3];

			for(int j = 0; j < 3; j++) 
				file.read ((char*)&polygons[i][j], sizeof(int)); // lee cada índice del triángulo (a,b,c)
		}
	}

	file.close();
}

// función que lee un archivo .bundles
void read_bundles(const string &path, int &nBundles, vector<int> &nFibers, vector<vector<int>> &nPoints, vector<vector<vector<vector<float>>>> &Points) {

	vector<string> bundlesDir;

	DIR *dir;
	struct dirent *ent;
	if((dir = opendir(path.c_str())) != NULL) {
	  /* get all the file names and directories within directory */
	  while((ent = readdir (dir)) != NULL) {
	    const string bundle_dir = ent->d_name;
	    if(*bundle_dir.rbegin() == 'a')
	    	bundlesDir.emplace_back(path + bundle_dir);
	  }
	  closedir (dir);
	}
	else {
	  /* could not open directory */
	  perror("Error");
	  exit( EXIT_FAILURE );
	}

	sort(bundlesDir.begin(), bundlesDir.end());

	nBundles = bundlesDir.size();
	nFibers = vector<int>(nBundles);
	nPoints = vector<vector<int>> (nBundles,vector<int>());
	Points =  vector<vector<vector<vector<float>>>> (nBundles);

	for(int i=0; i<nBundles; i++) {

		string filename = bundlesDir[i];
		ifstream file(filename, ios::in | ios::binary);

		if(file.is_open()) {

			streampos fsize = 0;
			file.seekg( 0, ios::end );
			int num = (file.tellg() - fsize) / 4;
			file.seekg( 0 );

			int num_count = 0;

			vector<int> nPointsList;
			vector<vector<vector<float>>> PointsList;

			while( num_count < num ) {

				int total_points; // number of points of each fiber
				file.read ((char*)&total_points, sizeof(int));

				vector<vector<float>> Fiber(total_points,vector<float>(3));

				for(int k=0; k<total_points; k++) {
					//Fiber[k] = new float[3];

					for(int m = 0; m < 3; m++)
						file.read ((char *) & Fiber[k][m], sizeof(float));
				}

				num_count = num_count + 1 + ( total_points * 3 );
				nPointsList.emplace_back(total_points);
				PointsList.emplace_back(Fiber);
			}

			nFibers[i] = nPointsList.size();
			nPoints[i] = vector<int> (nPointsList.size());
			Points[i] = vector<vector<vector<float>>> (nPointsList.size());
			//nPoints[i] = new int[nPointsList.size()];
			//Points[i] = new float**[nPointsList.size()];

			for(int j=0; j<(int)nPointsList.size(); j++) {
				nPoints[i][j] = nPointsList[j];
				Points[i][j] = PointsList[j];
			}
		}
		file.close();
	}
}

void write_intersection(const string &path, const string &pathBundles,
	const vector<vector<int>> &InTri, const vector<vector<int>> &FnTri,
	const vector<vector<vector<float>>> &InPoints, const vector<vector<vector<float>>> &FnPoints,
	const vector<vector<int>> &fib_index) {
	
	vector<string> bundlesDir;

	DIR *dirInt;
	struct dirent *ent;
	if((dirInt = opendir(pathBundles.c_str())) != NULL) {
	  /* get all the file names and directories within directory */
	  while((ent = readdir (dirInt)) != NULL) {
	    const string bundle_dir = ent->d_name;
	    if(*bundle_dir.rbegin() == 's')
	    	bundlesDir.emplace_back(bundle_dir.substr(0,bundle_dir.size() - 8));
	  }
	  closedir (dirInt);
	}
	else {
	  /* could not open directory */
	  perror("Error");
	  exit( EXIT_FAILURE );
	}

	sort(bundlesDir.begin(), bundlesDir.end());

	// ============================================================================

	DIR *dir;
	if((dir = opendir(path.c_str())) == NULL) { // Checks if a directory path exists

		const int dir_err = mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		if(dir_err == -1) {
			perror("Error creating directory!");
			exit( EXIT_FAILURE );
		}
	}
	closedir (dir);

	for(uint16_t i=0; i<fib_index.size(); i++) { //50
		ofstream file(path+"/" + bundlesDir[i] + ".txt", ios::out);
		if(file.is_open()) {

			const uint32_t len_fibInd = fib_index[i].size();
			file << len_fibInd;
			file <<"\n";
			for(uint32_t j=0; j<InTri[i].size(); j++) // writes index of each initial triangle
				file<<InTri[i][j]<<" ";
			file <<"\n";
			for(uint32_t j=0; j<FnTri[i].size(); j++) // writes index of each final triangle
				file<<(FnTri[i][j])<<" ";
			file <<"\n";
			for(uint32_t j=0; j<InPoints[i].size(); j++) { // writes the initial intersection point (x,y,z)
				for(uint8_t k=0; k<3; k++) 
					file<<(InPoints[i][j][k])<<" ";
			}
			file <<"\n";

			for(uint32_t j=0; j<FnPoints[i].size(); j++) { // writes the final intersection point (x,y,z)
				for(uint8_t k=0; k<3; k++) 
					file<<(FnPoints[i][j][k])<<" ";
			}
			file <<"\n";
			for(uint32_t j=0; j<len_fibInd; j++) // writes index of each fiber
				file<<(fib_index[i][j])<<" ";
			file <<"\n";
		}
		file.close();
	}
}
