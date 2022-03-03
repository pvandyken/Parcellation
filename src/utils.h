#pragma once
#include <Eigen/Dense>
#include <vector>

using namespace std;

namespace Utils {
    template <class T> bool compareVec(T a, T b) {
        for (int i = 0; i<3; i++) {
            if (a(i) != b(i)) {
            return a(i) < b(i);
            }
        }
        return false;
    };
}

template <class T>
class VecProxy {
private:
    vector<T>& v1, v2;
public:
    VecProxy(vector<T>& ref1, vector<T>& ref2) : v1(ref1), v2(ref2) {};
    const T& operator[](const size_t& i) const;
    const size_t size() const;
};

template <class T>
const T& VecProxy<T>::operator[](const size_t& i) const{
    return (i < v1.size()) ? v1[i] : v2[i - v1.size()];
};

template <class T>
const size_t VecProxy<T>::size() const { return v1.size() + v2.size(); };
