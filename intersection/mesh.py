from pathlib import Path
import nibabel as nib
import numpy as np
from numpy.typing import NDArray
from fury.utils import set_polydata_triangles, set_polydata_vertices, PolyData
from fury.io import save_polydata
import itertools as it
from intersection.cortical_intersections import MeshData


class Mesh:
    def __init__(self, vertices: NDArray[np.float32], polygons: NDArray[np.int32]):
        # Empty vertex arrays have a one dimensional shape, which will not be accepted
        # by MeshData, so we reshape it here
        if not len(vertices):
            vertices = vertices.reshape((0, 3))
        self.vertices = vertices
        self.polygons = polygons
        self.data = MeshData(vertices, polygons)

    def filter_triangles(self, whitelist: list[int]):
        whitelist_unique = list(set(whitelist))
        triangles = np.ndarray((len(whitelist_unique), 3), self.data.polygons.dtype)
        for i in range(len(triangles)):
            triangles[i] = self.data.polygons[whitelist_unique[i]]

        needed_points = sorted(set(it.chain.from_iterable(triangles)))
        indexed_points = dict(zip(needed_points, range(len(needed_points))))
        for triangle in triangles:
            for i, x in enumerate(triangle):
                triangle[i] = indexed_points[x]
        points = np.asarray(
            [self.data.vertices[i] for i in needed_points], self.data.vertices.dtype
        )
        return Mesh(points, triangles)


    def save_vtk(self, name: Path):
        pd = PolyData()
        set_polydata_triangles(pd, self.data.polygons)
        set_polydata_vertices(pd, self.data.vertices)
        save_polydata(pd, str(name), binary=True)


    @classmethod
    def load_mesh(cls, name: Path):
        data = nib.load(name)
        return cls(data.agg_data("pointset"), data.agg_data("triangle"))
