from pathlib import Path
import operator as op
import re
import itertools as it
from typing import Any

import typer
import networkx as nx

from intersection.cortical_intersections import CorticalIntersection, Bundle
from intersection.models import Results
from intersection.mesh import Mesh
from intersection.triangle_merge import merge_triangles, triangle_blob, fill_holes
from intersection.vtk_bundles import get_vtk_bundles

app = typer.Typer()


@app.command()
def main(
    left_surf: Path,
    # right_hem: Path,
    left_bundles: Path,
    # right_bundles: Path,
    out_path: Path,
    ray_length: int = 5,
    no_blobbing: bool = False,
):
    surf = Mesh.load_mesh(left_surf)
    intersections = get_intersection(left_surf, left_bundles)
    if no_blobbing:
        front = intersections.get_triangles_front()
        back = intersections.get_triangles_back()
    else:
        front = merge_and_fill(intersections.get_triangles_front(), surf)
        back = merge_and_fill(intersections.get_triangles_back(), surf)
    out_path.mkdir(exist_ok=True)
    for i, path in enumerate(
        filter(lambda x: re.search(r"\.tck$", str(x)), left_bundles.iterdir())
    ):
        src_triangles = surf.filter_triangles(front[i])
        dest_triangles = surf.filter_triangles(back[i])
        src_triangles.save_vtk(out_path / (path.with_suffix(".src.vtk").name))
        dest_triangles.save_vtk(out_path / (path.with_suffix(".dest.vtk").name))


def merge_and_fill(triangles: list[list[int]], mesh: Mesh):
    merged = [merge_triangles(intersection, mesh) for intersection in triangles]
    for intersection in merged:
        fill_holes(intersection, mesh)
    return merged


def get_intersection(surf_path: Path, bundles_path: Path):
    surf = Mesh.load_mesh(surf_path)
    bundles = get_vtk_bundles(bundles_path)

    intersections = CorticalIntersection(surf.data, bundles, 6)
    return intersections




@app.command()
def triangle(surface: Path, out: Path):
    mesh = Mesh.load_mesh(surface)
    blob = set()
    for layer in triangle_blob(455000, 3, mesh):
        blob = set(layer)
    mesh.filter_triangles(list(blob)).save_vtk(out)


if __name__ == "__main__":
    main(
        Path("data/mnt/smoothwm.surf.gii.gii"),
        Path("data/bundles-tck"),
        Path("data/mnt/out-dipy"),
    )
