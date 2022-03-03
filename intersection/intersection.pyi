from typing import Any
import numpy as np
def cortical_intersections(
    vertices: np.ndarray[Any, np.dtype[np.float32]],
    polygons: np.ndarray[Any, np.dtype[np.int32]],
    bundle_dir: str,
    out: str,
    ray_length: int
) -> int: ...