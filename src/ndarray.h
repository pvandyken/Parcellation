#ifndef NDARRAY_C
#define NDARRAY_C
#include <array>
#include <initializer_list>
#include <iostream>
#include <stdexcept>
#include <vector>

using namespace std;

template <class T> class Ndarray {
public:
  Ndarray(){};
  Ndarray(T *data, vector<int> shape, vector<int> strides)
      : shape{shape}, data{data}, strides{strides} {
    if (shape.size() != strides.size()) {
    }
  }

  T operator()(initializer_list<int> indices);
  T* slice(initializer_list<int> indices);
  void pprint();

  vector<int> shape;
private:
  T *data;
  vector<int> strides;
  void print_3rd();
  void print_2d(T* start);
  void print_1st(T* start);
};

template <class T> T Ndarray<T>::operator()(initializer_list<int> indices) {
  if (indices.size() != shape.size()) {
    throw invalid_argument(
        "Must supply number of indices equal to the number of dimensions.\n"
        "Got " +
        to_string(indices.size()) + " but expected " + to_string(shape.size()));
  }
  int index = 0;
  unsigned i = 0;
  for (auto dim_index : indices) {
    const int dim_length = shape[i];
    const int stride = strides[i];

    if (dim_index >= dim_length || dim_index < 0) {
      throw invalid_argument(
          "Index out of range.\nGot " + to_string(dim_index) +
          ". Expected index between 0 and " + to_string(dim_length));
    }
    index += stride * dim_index;
    i++;
  }
  return data[index];
}

template <class T> T* Ndarray<T>::slice(initializer_list<int> indices) {
  if (indices.size() >= shape.size()) {
    throw invalid_argument("Number of indices must be less than the number of dimensions");
  }
  if (indices.size() == 0) {
    throw invalid_argument("Must have at least one index");
  }

  vector<int> new_shape, new_strides;
  int index = 0;
  unsigned i = 0;
  new_shape = shape;
  new_strides = strides;
  for (auto dim_index : indices) {
    const int dim_length = shape[i];
    const int stride = strides[i];

    if (dim_index >= dim_length || dim_index < 0) {
      throw invalid_argument("Index out of range");
    }
    new_shape.erase(new_shape.begin());
    new_strides.erase(new_strides.begin());
    index += stride * dim_index;
    i++;
  }
  return &data[index];
}

template <class T> void Ndarray<T>::pprint() {
  if (shape.size() > 3) {
    cout << "Cannot print more than 3 dimensions" << endl;
    return;
  }
  if (shape.size() == 3) {
    print_3rd();
  } else if (shape.size() == 2)
  {
    print_2d(data);
  } else {
    print_1st(data);
  }
  cout << endl;
}

template <class T> void Ndarray<T>::print_3rd() {
  int n = shape.size();
  for (int i=0; i<shape[n-3]; i++) {
    print_2d(slice({i}));
    cout << "\n";
  }
}

template <class T> void Ndarray<T>::print_2d(T* start) {
  int n = shape.size();
  for (int i=0; i<shape[n-2]; i++) {
    print_1st(start + i*shape[n-1]);
    cout << "\n";
  }
}

template <class T> void Ndarray<T>::print_1st(T* start) {
  int n = shape.size();
  for (int i=0; i<shape[n-1]; i++) {
    cout << start[i] << " ";
  }
}

inline void read_first(Ndarray<float> arr) {
  cout << arr({0, 1, 0}) << "\n"
       << arr({1, 2}) << "\n"
       << arr({5, 1, 6}) << endl;
}
#endif
