# distutils: language = c++
from pathlib import Path
import numpy as np
from libcpp.vector cimport vector
from libcpp.string cimport string

cdef extern from "../ndarray.h":
    cdef cppclass Ndarray[T]:
        Ndarray() except+
        Ndarray(T*, vector[int], vector[int]) except+
        void pprint()
    
    void print_pointer(int*)

cdef extern from '../entry.h':
    int cortical_intersections(Ndarray[float], Ndarray[int], string, string)

cdef vector[int] to_vector(items):
    cdef vector[int] vec
    for item in items:
        vec.push_back(item)
    return vec

cdef vector[int] c_strides(arr):
    cdef vector[int] cstrides
    for stride in arr.strides:
        cstrides.push_back(stride / arr.itemsize)
    return cstrides

def _c_ndarray(arr):
    cdef vector[int] shape = to_vector(arr.shape)
    cdef vector[int] strides = c_strides(arr)
    return arr, shape, strides

cdef Ndarray[int] int_ndarray(arr):
    cdef vector[int] shape, strides
    arr, shape, strides = _c_ndarray(arr)

    cdef int[::1] memview = arr.ravel()

    cdef Ndarray[int] narr = Ndarray[int](&memview[0], shape, strides)
    return narr


cdef Ndarray[float] float_ndarray(arr):
    cdef vector[int] shape, strides
    arr, shape, strides = _c_ndarray(arr)

    cdef float[::1] memview = arr.ravel()

    cdef Ndarray[float] narr = Ndarray[float](&memview[0], shape, strides)
    return narr

def intersections(vertices, triangles, bundles_dir, out_path):
    if len(vertices.shape) != 2 or vertices.shape[1] != 3:
        raise Exception("Vertex array must have 2 dimensions with shape (x, 3)")

    if len(triangles.shape) != 2 or triangles.shape[1] != 3:
        raise Exception("triangles array must have 2 dimensions with shape (x, 3)")
    vertices = np.ascontiguousarray(vertices)
    triangles = np.ascontiguousarray(triangles)
    cdef Ndarray[int] triangle_arr = int_ndarray(triangles)
    cdef Ndarray[float] vertix_arr = float_ndarray(vertices)

    cdef int ptsline = 2
    cortical_intersections(
        vertix_arr, triangle_arr, bundles_dir.encode(), out_path.encode()
    )
