import itertools as it
from pathlib import Path
from typing import Type, TypeVar

import attrs
import nibabel as nib
import numpy as np
from nibabel.freesurfer.io import read_annot
from fury.io import save_polydata
from fury.utils import PolyData, set_polydata_triangles, set_polydata_vertices
from numpy.typing import NDArray

T = TypeVar("T")


@attrs.frozen
class Results:
    num_fibers: int
    init_triangles: list[int]
    end_triangles: list[int]
    init_intersection_points: list[float]
    end_intersection_points: list[float]
    fiber_indices: list[int]

    @classmethod
    def _get_list(cls, line: str, converter: Type[T] = int) -> list[T]:
        return [converter(x) for x in line.split()]  # type: ignore

    @classmethod
    def from_out_file(cls, file: Path):
        with file.open("r") as f:
            data = f.read().split("\n")
        return cls(
            int(data[0]),
            cls._get_list(data[1]),
            cls._get_list(data[2]),
            cls._get_list(data[3], float),
            cls._get_list(data[4], float),
            cls._get_list(data[5]),
        )




@attrs.frozen
class Mesh:
    points: NDArray[np.float32]
    triangles: NDArray[np.int32]

    def filter_triangles(self, whitelist: list[int]):
        triangles = np.asarray(
            [tri for i, tri in enumerate(self.triangles) if i in set(whitelist)],
            self.triangles.dtype
        )
        needed_points = sorted(set(it.chain.from_iterable(triangles)))
        indexed_points = dict(zip(needed_points, range(len(needed_points))))
        for triangle in triangles:
            for i, x in enumerate(triangle):
                triangle[i] = indexed_points[x]
        points = np.asarray([self.points[i] for i in needed_points], self.points.dtype)
        return self.__class__(
            points,
            triangles
        )

    def save_vtk(self, name: Path):
        pd = PolyData()
        set_polydata_triangles(pd, self.triangles)
        set_polydata_vertices(pd, self.points)
        save_polydata(pd, str(name), binary=True)

    def get_labels(self, annotations: "Annotations", triangles: list[int]):
        for i in triangles:
            triangle = self.triangles[i]
            labels = [annotations.get_label(coord) for coord in triangle]
            yield labels

    @classmethod
    def from_file(cls, name: Path):
        data = nib.load(name)
        return cls(
            points=data.agg_data("pointset"), triangles=data.agg_data("triangle")
        )


@attrs.frozen()
class Annotations:
    labels: NDArray[np.int32]
    ctab: NDArray[np.int32]
    names: list[str]

    def get_label(self, coord: int):
        return self.names[self.labels[coord]]

    @classmethod
    def load(cls, path: Path):
        labels, ctab, names = read_annot(path)
        return cls(
            labels,
            ctab,
            [s.decode() for s in names]
        )
