#include "bundles.h"

Bundle::Bundle(const string& path) {
    ifstream file(path, ios::in | ios::binary);
    cout << "Reading file: " << path << endl;

    if (file.is_open()) {

      streampos fsize = 0;
      file.seekg(0, ios::end);
      int num = (file.tellg() - fsize) / 4;
      file.seekg(0);

      int num_count = 0;

      vector<int> nPointsList;
      vector<vector<Vector3f>> PointsList;

      while (num_count < num) {

        int total_points; // number of points of each fiber
        file.read((char *)&total_points, sizeof(int));

        vector<Vector3f> Fiber(total_points, Vector3f());

        for (int k = 0; k < total_points; k++) {
          for (int m = 0; m < 3; m++)
            file.read((char *)&Fiber[k][m], sizeof(float));
        }

        num_count = num_count + 1 + (total_points * 3);
        nPointsList.emplace_back(total_points);
        PointsList.emplace_back(Fiber);
      }

      this->fibers = vector<vector<Vector3f>>(nPointsList.size());

      for (int i = 0; i < (int)nPointsList.size(); i++) {
        this->fibers[i] = PointsList[i];
      }
    }
    file.close();
}