#pragma once
#include <Eigen/Dense>


namespace Utils {
    template <class T> bool compareVec(T a, T b) {
    for (int i = 0; i<3; i++) {
        if (a(i) != b(i)) {
        return a(i) < b(i);
        }
    }
    return false;
    }
}
