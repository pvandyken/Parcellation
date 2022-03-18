from typing import Any, overload
import numpy as np
import networkx as nx

def intersection_core(
    vertices: np.ndarray[Any, np.dtype[np.float32]],
    polygons: np.ndarray[Any, np.dtype[np.int32]],
    bundle_dir: str,
    # out: str,
    ray_length: int,
    probability_threshold: float,
) -> tuple[list[list[int]], list[list[int]]]: ...

class CorticalIntersection:
    def __init__(self, mesh: "MeshData", bundles: list["Bundle"], ray_length: int): ...
    @staticmethod
    def from_bundles(
        mesh: "MeshData",
        bundle_dir: str,
        ray_length: int,
        align_orientation: bool = ...,
    ) -> CorticalIntersection: ...
    def get_globbed_graph(self, radius=2) -> nx.DiGraph: ...

class MeshData:
    def __init__(
        self,
        vertices: np.ndarray[Any, np.dtype[np.float32]],
        polygons: np.ndarray[Any, np.dtype[np.int32]],
    ): ...
    vertices: np.ndarray[Any, np.dtype[np.float32]]
    polygons: np.ndarray[Any, np.dtype[np.int32]]

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
