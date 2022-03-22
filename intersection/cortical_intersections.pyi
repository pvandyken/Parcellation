from pathlib import Path
from typing import Any, Iterator, List, Union, overload, Tuple

import networkx as nx
import numpy as np
from numpy.typing import NDArray
from fury.utils import PolyData


def intersection_core(
    vertices: np.ndarray[Any, np.dtype[np.float32]],
    polygons: np.ndarray[Any, np.dtype[np.int32]],
    bundle_dir: str,
    # out: str,
    ray_length: int,
    probability_threshold: float,
) -> tuple[list[list[int]], list[list[int]]]: ...

class CorticalIntersection:
    def __init__(self, mesh: "Mesh", bundles: list["Bundle"], ray_length: int): ...
    @property
    def triangles(self) -> Tuple[List[List[int]], List[List[int]]]: ...
    @staticmethod
    def from_bundles(
        mesh: "Mesh",
        bundle_dir: str,
        ray_length: int,
        align_orientation: bool = ...,
    ) -> CorticalIntersection: ...
    def get_globbed_graph(self, radius=2) -> nx.DiGraph: ...

class Mesh:
    def __init__(self, name: Union[str, Path]): ...
    vertices: np.ndarray[Any, np.dtype[np.float32]]
    polygons: np.ndarray[Any, np.dtype[np.int32]]

    @property
    def polydata(self) -> PolyData: ...
    def save_vtk(self, path: Union[str, Path], binary: bool = ...) -> None: ...
    def filter_triangles(self, whitelist: list[int]) -> Mesh: ...
    @overload
    def get_triangle_neighbors(
        self, triangle: int, edge_adjacent: bool = ...
    ) -> list[int]: ...
    @overload
    def get_triangle_neighbors(self, triangle: list[int]) -> list[int]: ...
    @overload
    def get_triangle_neighbors(
        self, triangle: list[int], exclude: list[int]
    ) -> list[int]: ...
    @overload
    def get_triangles_of_point(self, point: int) -> list[int]: ...
    @overload
    def get_triangles_of_point(self, point: list[int]) -> list[int]: ...

class Bundle:
    def __init__(self, fibers: list[list[np.ndarray[Any, np.dtype[np.float32]]]]): ...

class Parcellation:
    def __init__(self, parcels: list[set[int]]): ...
    def get_connectome(
        self, intersections: CorticalIntersection
    ) -> NDArray[np.int32]: ...
    def __iter__(self) -> Iterator[set[int]]: ...
