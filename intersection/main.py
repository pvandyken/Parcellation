from pathlib import Path

import typer
import fury.io as fio

from intersection.cortical_intersections import CorticalIntersection
from intersection.mesh import Mesh
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
):
    surf = Mesh.load_mesh(left_surf)
    G = get_intersection(left_surf, left_bundles)
    atlas = merge_parcels(get_parcellation(G, surf), surf)
    fio.save_polydata(atlas, str(out_path))


def get_intersection(surf_path: Path, bundles_path: Path):
    surf = Mesh.load_mesh(surf_path)
    bundles = get_vtk_bundles(bundles_path)

    return CorticalIntersection(surf.data, bundles, 6).get_globbed_graph(2)






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
        Path("data/mnt/l_hem-tck"),
        Path("data/mnt/out-full-atlas.vtk"),
    )
