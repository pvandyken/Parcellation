from typing import Any, overload
import numpy as np

def intersection_core(
    vertices: np.ndarray[Any, np.dtype[np.float32]],
    polygons: np.ndarray[Any, np.dtype[np.int32]],
    bundle_dir: str,
    # out: str,
    ray_length: int,
    probability_threshold: float,
) -> tuple[list[list[int]], list[list[int]]]: ...

class CorticalIntersection:
    def __init__(self): ...
    @staticmethod
    def from_bundles(
        mesh: "MeshData",
        bundle_dir: str,
        ray_length: int,
    ) -> CorticalIntersection: ...
    def get_triangles_front(self) -> list[list[int]]: ...
    def get_triangles_back(self) -> list[list[int]]: ...

class MeshData:
    def __init__(
        self,
        vertices: np.ndarray[Any, np.dtype[np.float32]],
        polygons: np.ndarray[Any, np.dtype[np.int32]],
    ): ...
    vertices: np.ndarray[Any, np.dtype[np.float32]]
    polygons: np.ndarray[Any, np.dtype[np.int32]]

    @overload
    def get_triangle_neighbors(self, triangle: int) -> list[int]: ...
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