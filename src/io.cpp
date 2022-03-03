#include "io.h"
#include <iostream>

// función que lee el mallado (vértices y polígonos de los triángulos)
void IO::read_mesh(const string &filename, vector<vector<float>> &vertex,
               vector<vector<int>> &polygons, int &len_vertex,
               int &len_polygons) {

  ifstream file(filename, ios::in | ios::binary);

  if (file.is_open()) {

    file.seekg(17);

    int dim;

    file.read((char *)&dim, sizeof(int)); // dimensiones

    file.seekg(29);

    file.read((char *)&len_vertex, sizeof(int)); // largo de vértices

    std::cout << "Made it here!\n" << len_vertex << std::endl;
    vector<vector<float>> vertex(len_vertex, vector<float>(3));

    std::cout << "before" << std::endl;
    std::cout << len_vertex << std::endl;
    std::cout << "after" << std::endl;
    for (int i = 0; i < len_vertex; i++) {
      std::cout << "made it here!" << std::endl;

      // vertex[i] = new float[3];

      for (int j = 0; j < 3; j++)
        file.read((char *)&vertex[i][j],
                  sizeof(float)); // lee cada punto (x,y,z)
    }

    file.seekg(4 * 3 * len_vertex + 41);

    file.read((char *)&len_polygons, sizeof(int)); // largo de polígonos
    vector<vector<int>> polygons(len_polygons, vector<int>(3));

    for (int i = 0; i < len_polygons; i++) {
      // polygons[i] = new int[3];

      for (int j = 0; j < 3; j++)
        file.read((char *)&polygons[i][j],
                  sizeof(int)); // lee cada índice del triángulo (a,b,c)
    }
  }

  file.close();
}

// función que lee un archivo .bundles
void IO::read_bundles(const string &path, vector<Bundle> &bundles) {
  vector<string> bundlesDir;

  DIR *dir;
  struct dirent *ent;
  const string dir_name = (path.back() == '/') ? path : path + "/";
  if ((dir = opendir(path.c_str())) != NULL) {
    /* get all the file names and directories within directory */
    while ((ent = readdir(dir)) != NULL) {
      const string bundle_dir = ent->d_name;
      if (*bundle_dir.rbegin() == 'a')
        bundlesDir.emplace_back(dir_name + bundle_dir);
    }
    closedir(dir);
  } else {
    /* could not open directory */
    perror("Error");
    exit(EXIT_FAILURE);
  }

  sort(bundlesDir.begin(), bundlesDir.end());

  for (int i = 0; i < bundlesDir.size(); i++) {
    string filename = bundlesDir[i];
    bundles.push_back(Bundle(filename));
  }
}

void IO::write_intersection(const string &path, const string &pathBundles,
                        const vector<BundleIntersection> &frontIntersections,
                        const vector<BundleIntersection> &backIntersections,
                        const vector<vector<int>> &fib_index) {

  vector<string> bundlesDir;

  DIR *dirInt;
  struct dirent *ent;
  if ((dirInt = opendir(pathBundles.c_str())) != NULL) {
    /* get all the file names and directories within directory */
    while ((ent = readdir(dirInt)) != NULL) {
      const string bundle_dir = ent->d_name;
      if (*bundle_dir.rbegin() == 's')
        bundlesDir.emplace_back(bundle_dir.substr(0, bundle_dir.size() - 8));
    }
    closedir(dirInt);
  } else {
    /* could not open directory */
    perror("Error");
    exit(EXIT_FAILURE);
  }

  sort(bundlesDir.begin(), bundlesDir.end());

  // ============================================================================

  DIR *dir;
  if ((dir = opendir(path.c_str())) ==
      NULL) { // Checks if a directory path exists

    const int dir_err =
        mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if (dir_err == -1) {
      perror("Error creating directory!");
      exit(EXIT_FAILURE);
    }
  }
  closedir(dir);

  for (uint16_t i = 0; i < fib_index.size(); i++) { // 50
    ofstream file(path + "/" + bundlesDir[i] + ".txt", ios::out);
    if (file.is_open()) {

      const uint32_t len_fibInd = fib_index[i].size();
      file << len_fibInd;
      file << "\n";
      for (auto triangle : frontIntersections[i].triangles) // writes index of each initial triangle
        file << triangle << " ";
      file << "\n";
      for (auto triangle : backIntersections[i].triangles) // writes index of each final triangle
        file << triangle << " ";
      file << "\n";
      for (auto point : frontIntersections[i].points) { // writes the initial intersection point (x,y,z)
        for (auto coord : point)
          file << coord << " ";
      }
      file << "\n";

      for (auto point : backIntersections[i].points) { // writes the final intersection point (x,y,z)
        for (auto coord : point)
          file << coord << " ";
      }
      file << "\n";
      for (uint32_t j = 0; j < len_fibInd; j++) // writes index of each fiber
        file << (fib_index[i][j]) << " ";
      file << "\n";
    }
    file.close();
  }
}
