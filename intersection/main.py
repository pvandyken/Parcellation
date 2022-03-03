from pathlib import Path

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
    return get_intersection(left_surf, left_bundles, out_path)


def get_intersection(surf_path: Path, bundles_path: Path, out_path: Path) -> int:
    surf = Mesh.from_file(surf_path)
    return intersection.cortical_intersections(
        surf.points, surf.triangles, str(bundles_path), str(out_path), 6
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
