from pathlib import Path
import re

import typer

from intersection import intersection
from intersection.models import Mesh, Results

app = typer.Typer()


@app.command()
def main(
    left_surf: Path,
    # right_hem: Path,
    left_bundles: Path,
    # right_bundles: Path,
    out_path: Path,
    ray_length: int = 5
):
    front_core, back_core = get_intersection(left_surf, left_bundles)
    surf = Mesh.from_file(left_surf)
    out_path.mkdir(exist_ok=True)
    for i, path in enumerate(filter(
        lambda x: re.search(r"\.bundles$", str(x)), left_bundles.iterdir()
    )):
        src_triangles = surf.filter_triangles(front_core[i])
        dest_triangles = surf.filter_triangles(back_core[i])
        src_triangles.save_vtk(out_path/(path.with_suffix(".src.vtk").name))
        dest_triangles.save_vtk(out_path/(path.with_suffix(".dest.vtk").name))
        # print(out_path/(path.with_suffix(".src.vtk").name))
        # print(out_path/(path.with_suffix(".dest.vtk").name))


def get_intersection(surf_path: Path, bundles_path: Path):
    surf = Mesh.from_file(surf_path)
    return intersection.intersection_core(
        surf.points, surf.triangles, str(bundles_path), 6, 0.05
    )


@app.command()
def test(txt_file: Path, surface: Path, out_dir: Path):
    results = Results.from_out_file(txt_file)
    surf = Mesh.from_file(surface)
    src_triangles = surf.filter_triangles(results.init_triangles)
    dest_triangles = surf.filter_triangles(results.end_triangles)
    out_dir.mkdir(exist_ok=True)
    src_triangles.save_vtk(out_dir/"src.vtk")
    dest_triangles.save_vtk(out_dir/"dest.vtk")
