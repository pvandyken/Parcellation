from pathlib import Path
import re
import itertools as it
from typing import Any

import typer
import networkx as nx

from intersection.cortical_intersections import CorticalIntersection
from intersection.models import Results
from intersection.mesh import Mesh
from intersection.triangle_merge import merge_triangles

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
    surf = Mesh.load_mesh(left_surf)
    intersections = get_intersection(left_surf, left_bundles)
    # front = merge_triangles(intersections.get_triangles_front()[0], surf)
    # back = merge_triangles(intersections.get_triangles_back()[0], surf)
    front = intersections.get_triangles_front()
    back = intersections.get_triangles_back()
    out_path.mkdir(exist_ok=True)
    for i, path in enumerate(filter(
        lambda x: re.search(r"\.bundles$", str(x)), left_bundles.iterdir()
    )):
        src_triangles = surf.filter_triangles(front[i])
        dest_triangles = surf.filter_triangles(back[i])
        src_triangles.save_vtk(out_path/(path.with_suffix(".src.vtk").name))
        dest_triangles.save_vtk(out_path/(path.with_suffix(".dest.vtk").name))
        # print(out_path/(path.with_suffix(".src.vtk").name))
        # print(out_path/(path.with_suffix(".dest.vtk").name))


def get_intersection(surf_path: Path, bundles_path: Path):
    surf = Mesh.load_mesh(surf_path)
    intersections = CorticalIntersection.from_bundles(
        surf.data, str(bundles_path), 6
    )
    return intersections


def get_fiber_graph(intersections: CorticalIntersection, threshold: float):
    fronts = intersections.get_triangles_front()
    backs = intersections.get_triangles_back()

    G = nx.Graph()

    for i, front_back in enumerate(zip(fronts, backs)):
        front, back = front_back
        G.add_node(i, front=front, back=back)

    for a, b in it.combinations(G, 2):
        for a_inter, b_inter in it.product(G.nodes[a].values(), G.nodes[b].values()):
            if get_overlap(a_inter, b_inter) > threshold:
                G.add_edge(a, b)
                break
    return G



def get_overlap(x: list[Any], y: list[Any]):
    return len(set(x).intersection(y)) / min(len(set(x)), len(set(y)))


@app.command()
def test(txt_file: Path, surface: Path, out_dir: Path):
    results = Results.from_out_file(txt_file)
    surf = Mesh.from_file(surface)
    src_triangles = surf.filter_triangles(results.init_triangles)
    dest_triangles = surf.filter_triangles(results.end_triangles)
    out_dir.mkdir(exist_ok=True)
    src_triangles.save_vtk(out_dir/"src.vtk")
    dest_triangles.save_vtk(out_dir/"dest.vtk")

if __name__ == "__main__":
    intersection = get_intersection("data/mnt/smoothwm.surf.gii.gii", "data/mnt/sup-f-L")
    nx.write_gml(get_fiber_graph(intersection, 0.1), "data/mnt/graph.gml")