from typing import List
from pathlib import Path

import typer
import fury.io as fio

from intersection.cortical_intersections import CorticalIntersection, Mesh, Parcellation
from intersection.patch_merge import get_parcellation, merge_parcels
from intersection.triangle_merge import triangle_blob
from intersection.vtk_bundles import get_vtk_bundles

app = typer.Typer()


@app.command()
def main(
    left_surf: Path,
    # right_hem: Path,
    left_bundles: Path,
    # right_bundles: Path,
    out_path: Path,
    raw: bool = False,
    threads: int = 1
):
    surf = Mesh(left_surf)
    intersection = get_intersection(left_surf, left_bundles, threads)
    if raw:
        save_src_dest(surf, out_path, *intersection.triangles)
        return 0

    G = intersection.get_globbed_graph(2)
    parcels = get_parcellation(G, surf)
    atlas = merge_parcels(parcels, surf)
    fio.save_polydata(atlas, str(out_path))
    return 0
    # return Parcellation(parcels)


def save_src_dest(
    mesh: Mesh, out_path: Path, fronts: List[List[int]], backs: List[List[int]]
):
    for i, front, back in zip(range(len(fronts)), fronts, backs):
        mesh.filter_triangles(front).save_vtk(out_path / f"cluster_{i}.src.vtk")
        mesh.filter_triangles(back).save_vtk(out_path / f"cluster_{i}.dest.vtk")


def get_intersection(mesh: Path, bundles_path: Path, threads: int = 1):
    bundles = get_vtk_bundles(bundles_path)

    return CorticalIntersection(Mesh(mesh), bundles, 6, threads=threads)


@app.command()
def triangle(surface: Path, out: Path):
    mesh = Mesh(surface)
    blob = set()
    for layer in triangle_blob(455000, 3, mesh):
        blob = set(layer)
    mesh.filter_triangles(list(blob)).save_vtk(out)


if __name__ == "__main__":
    main(
        Path("data/mnt/smoothwm.surf.gii.gii"),
        Path("data/mnt/l_hem-tck"),
        Path("data/mnt/out-full-atlas.vtk"),
    )
