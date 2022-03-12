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


def get_overlap_graph(fronts: list[list[int]], backs: list[list[int]], threshold: float):
    G = nx.DiGraph()

    for i, triangles in enumerate(it.chain(fronts, backs)):
        G.add_node(
            i,
            triangles=triangles,
            index=i % len(fronts),
            position="front" if i < len(fronts) else "back",
            size=len(triangles),
        )

    for a, b in it.combinations(G, 2):
        if G.nodes[a]["index"] != G.nodes[b]["index"]:
            a_triangles = G.nodes[a]["triangles"]
            b_triangles = G.nodes[b]["triangles"]
            if len(a_triangles) > len(b_triangles):
                order = (a, b)
            else:
                order = (b, a)
            G.add_edge(*order, weight=get_overlap(a_triangles, b_triangles))

    for node in sorted(G.nodes(data="size"), key=op.itemgetter(1)):  # type: ignore
        edges = sorted(
            G.in_edges(node[0], data="weight"),  # type: ignore
            key=op.itemgetter(2),
            reverse=True
        )
        if not len(edges):
            continue
        for edge in edges[1:]:
            G.remove_edge(*edge[:2])
        
        try:
            if edges[0][2] < threshold:  # type: ignore
                G.remove_edge(*edges[0][:2])
        except Exception as E:
            print(edges)
            raise E

    return G


def get_overlap(x: list[Any], y: list[Any]):
    return len(set(x).intersection(y)) / min(len(set(x)), len(set(y)))


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
