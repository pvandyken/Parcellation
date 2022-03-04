from typing import Any
import numpy as np
def intersection_core(
    vertices: np.ndarray[Any, np.dtype[np.float32]],
    polygons: np.ndarray[Any, np.dtype[np.int32]],
    bundle_dir: str,
    out: str,
    ray_length: int,
    probability_threshold: float
) -> tuple[list[list[int]], list[list[int]]]: ...