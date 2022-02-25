# distutils: language = c++
import numpy as np
from libcpp.vector cimport vector

# cdef extern from '<array>' namespace "std":
#     cdef cppclass array4 "std:array<int, 4>":
#         array4() except+

cdef extern from "ndarray.h":
    cdef cppclass Ndarray[T]:
        Ndarray() except+
        Ndarray(T*, vector[int], vector[int]) except+
        void pprint()

cdef extern from 'entry.h':
    int cortical_intersections(Ndarray[float], Ndarray[int])
# cdef extern from '<vector>' namespace "std":
#     cdef cppclass vector[T]:
#         vector()

def to_vector(items):
    cdef vector[int] vec
    for item in items:
        vec.push_back(item)
    return vec

def c_strides(arr):
    cdef vector[int] cstrides
    for stride in arr.strides:
        cstrides.push_back(stride / arr.itemsize)
    return cstrides

def _c_ndarray(arr):
    if not arr.flags["C_CONTIGUOUS"]:
        arr = np.ascontiguousarray(arr)

    cdef vector[int] shape = to_vector(arr.shape)
    cdef vector[int] strides = c_strides(arr)
    return arr, shape, strides

cdef Ndarray[int] int_ndarray(arr):
    cdef vector[int] shape, strides
    arr, shape, strides = _c_ndarray(arr)

    cdef int[::1] memview = arr.flatten()

    cdef Ndarray[int] narr = Ndarray[int](&memview[0], shape, strides)
    return narr


cdef Ndarray[float] float_ndarray(arr):
    cdef vector[int] shape, strides
    arr, shape, strides = _c_ndarray(arr)

    cdef float[::1] memview = arr.flatten()

    cdef Ndarray[float] narr = Ndarray[float](&memview[0], shape, strides)
    return narr

def intersections(vertices, triangles):
    cdef Ndarray[int] triangle_arr = int_ndarray(triangles)
    cdef Ndarray[float] vertix_arr = float_ndarray(vertices)



    cdef int ptsline = 2
    cortical_intersections(vertix_arr, triangle_arr)

    # for fiber in FnPoints:
    #     for point in fiber:
    #         for coord in point:
    #             print(coord)
    #         print("")
    #     print("")

vertices = np.asarray([
    [0, 0, 0],
    [50, 0, 0],
    [0, 50, 0]
], dtype=np.single)
triangles = np.asarray([[0, 1, 2]], dtype=np.intc)

intersections(vertices, triangles)
