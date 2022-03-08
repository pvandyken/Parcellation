import itertools as it
import attr
from numpy.typing import NDArray
import numpy as np
from typing import Iterable,  Optional
import more_itertools as itx


def make_2d_grid(
    x: int, y: int, const: Optional[int] = None, const_pos: Optional[int] = None
) -> Iterable[Iterable[tuple[int, ...]]]:
    x_windows = itx.windowed(range(x), 2, fillvalue=0)
    y_windows = itx.windowed(range(y), 2, fillvalue=0)
    product = it.product(x_windows, y_windows)

    def insert_const(
        product: Iterable[tuple[int, int]],
        const: Optional[int],
        const_pos: Optional[int],
    ):
        for coord in product:
            if const is not None and const_pos is not None:
                coord_list = list(coord)
                coord_list.insert(const_pos, const)
                yield tuple(coord_list)
                continue
            yield coord

    return (
        insert_const(it.product(x_coords, y_coords), const, const_pos)
        for x_coords, y_coords in product
    )


def make_layered_grid(x: int, y: int, z: int, const_dim: int = 0):
    dims = [x, y, z]
    const = dims[const_dim]
    dims.pop(const_dim)
    grid = [make_2d_grid(*dims, const=i, const_pos=const_dim) for i in range(const)]
    return np.asarray(
        [[list(coord) for coord in square] for layer in grid for square in layer]
    )


def make_3d_grid(x: int, y: int, z: int):
    return np.concatenate([make_layered_grid(x, y, z, const_dim = dim) for dim in range(3)])


@attr.frozen()
class PositionIndex:
    array: NDArray[np.float32]
    strides: tuple[int,int,int]

    def get_index(self, x: int, y: int, z: int):
        return sum(i * stride for i, stride in zip([x, y, z], self.strides))


if __name__ == "__main__":
    import pprint
    pprint.pprint(make_3d_grid(2, 3, 2))
