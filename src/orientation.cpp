#include "orientation.h"

void Orientation::alignOrientation(vector<vector<Vector3f>> &fibers) {
    cout << "Aligning orientation..." << endl;
    vector<bool> directions;

    // Use first fiber as our reference. It's aligned by definition, so we add true as
    // the first entry to directions
    const vector<Vector3f> &refFiber = fibers[0];
    directions.push_back(true);

    // Loop through all other fibers and check if they're aligned to the reference.
    // Store the results in the directions array. Keep track of the number of aligned
    // and misaligned fibers
    int alignments = 0;
    int misalignments = 0;
    for (int i=1; i<fibers.size(); i++) {
        bool aligned = isAligned(fibers[i], refFiber);
        directions.push_back(aligned);
        if (aligned) {
            alignments++;
        } else {
            misalignments++;
        }
    }
    cout << "Misaligned: " << misalignments << endl;
    cout << "Aligned: " << alignments << endl;


    // If more fibers are misaligned than aligned, use the misaligned direction as
    // "true", so that all other fibers will be flipped to that direction
    const bool baseDirection = alignments > misalignments;

    cout << "Base direction: " << (baseDirection ? "True" : "False") << endl;

    vector<int> flippedFibers;
    for (int i=0; i<directions.size(); i++) {
        // If the fiber direction is different than the base direction, flip it
        if (baseDirection ^ directions[i]) {
            reverse(fibers[i].begin(), fibers[i].end());
            flippedFibers.push_back(i);
        }
    }

    cout << "Flipped fibers: \n";
    for (auto fiber : flippedFibers) {
        cout << fiber << " ";
    }
    cout << endl;
}


bool Orientation::isAligned(const vector<Vector3f> &fiber, const vector<Vector3f> &reference) {
    const Vector3f &refPoint = reference[0];
    float startDist = (fiber.front() - refPoint).norm();
    float endDist = (fiber.back() - refPoint).norm();
    return startDist < endDist;
}
