#include "mesh.h"

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

const vector<int> Mesh::getTriangleNeighbors(int triangle) {
    set<int> neighbors;
    for (auto point : polygons(triangle, all)) {
        const vector<int> &trianglesOfPoint = getTrianglesOfPoint(point);
        neighbors.insert(trianglesOfPoint.begin(), trianglesOfPoint.end());
    }
    return vector<int>(neighbors.begin(), neighbors.end());
}