#include "src/orientation.h"
#include <array>
#include <iostream>
#include <vector>
#include <unordered_set>

using namespace std;

template <class T> bool comp_v3(T a, T b) {
  for (int i = 0; i<3; i++) {
    if (a(i) != b(i)) {
      return a(i) < b(i);
    }
  }
  return false;
}

int main() {

  Vector3f a(1.2,1.6,2.3);
  Vector3f aa(1342,21,-23);
  Vector3f b(1.2, 1.6,2.3);
  Vector3f c(3,5,2);
  Vector3f d(1342, 234, -234);
  Vector3f e(1342, 21, -23);

  
  vector<Vector3f> all = {a, aa, b, c, d,e };

  sort(all.begin(), all.end(), &comp_v3<Vector3f>);
  all.erase(unique(all.begin(), all.end()), all.end());
  
  for (auto vec : all) {
    cout << vec.transpose() << endl;
  }

  // unordered_set<Vector3f> s = {a,b,c};
  // for (auto x : s) {
  //   cout << x.transpose() << endl;
  // }

}
