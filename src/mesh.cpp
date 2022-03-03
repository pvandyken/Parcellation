#include "mesh.h"
#include <iostream>

const vector<int>& Mesh::getTrianglesOfPoint(int point) {
    if (this->trianglesOfPointsIndex.size() == 0) {
        // Loop through every vertex; vertices are labelled by their position in the
        // matrix
        for (int i = 0; i < this->vertices.rows(); i++) {
            this->trianglesOfPointsIndex.push_back(vector<int>());

            // Loop through every triangle. Triangles are also labelled by their
            // position in the matrix.
            int triangleIndex = 0;
            for (Vector3i polygon : this->polygons.rowwise()) {
                auto index = find(polygon.begin(), polygon.end(), i);
                if (index != polygon.end()) {
                    this->trianglesOfPointsIndex.back().push_back(triangleIndex);
                }
                triangleIndex++;
            }
        }
    }

    return this->trianglesOfPointsIndex[point];
}