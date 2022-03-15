from collections import UserList
import copy
import itertools as it
import random
import more_itertools as itx
import vtk
import operator as op
from typing import Any, Iterable, Optional, Sequence, Union

import networkx as nx

from intersection.mesh import Mesh


def merge_overlapping(G: nx.DiGraph, threshold: int) -> tuple[set[int], ...]:
    def recurse(root: int) -> tuple[set[int], ...]:
        root_triangles: set[int] = set(G.nodes[root]["triangles"])
        all_triangles = copy.copy(root_triangles)
        secondary_patches: list[set[int]] = []
        child_secondaries: list[set[int]] = []
        for child in G.successors(root):
            if G.out_degree(child):
                merged, *child_secondaries = recurse(child)
                if not get_overlap(merged, root_triangles) > threshold:
                    secondary_patches.append(merged)
                    continue
                all_triangles.update(merged)
                continue
            all_triangles.update(G.nodes[child]["triangles"])
        return all_triangles, *secondary_patches, *child_secondaries

    if len(G) > 1:
        root = itx.one(set(G.nodes) - set(list(zip(*G.edges))[1]))
        return recurse(root)
    return (set(itx.one(G.nodes(data="triangles"))[1]),)  # type: ignore



AnyGraph = Union[nx.DiGraph, nx.Graph]


def segment_nodes(segments: list[Sequence[int]]):
    G = nx.DiGraph()
    for i, triangles in enumerate(segments):
        G.add_node(i, triangles=triangles, index=i, size=len(triangles))
    return G


def add_overlap_connections(G: AnyGraph):
    for a, b in it.combinations(G, 2):
        if G.nodes[a]["index"] != G.nodes[b]["index"]:
            a_triangles = G.nodes[a]["triangles"]
            b_triangles = G.nodes[b]["triangles"]
            if len(a_triangles) > len(b_triangles):
                order = (a, b)
            else:
                order = (b, a)
            G.add_edge(*order, weight=get_overlap(a_triangles, b_triangles))


def threshold_graph(G: AnyGraph, threshold: int):
    to_remove: set[tuple[int, int]] = set()
    for u, v, weight in G.edges(data="weight"):  # type: ignore
        if weight <= threshold:  # type: ignore
            to_remove.add((u, v))
    for edge in to_remove:
        G.remove_edge(*edge)


def get_overlap_graph(G: nx.DiGraph, threshold: float):
    add_overlap_connections(G)

    for node in sorted(G.nodes(data="size"), key=op.itemgetter(1)):  # type: ignore
        edges = sorted(
            G.in_edges(node[0], data="weight"),  # type: ignore
            key=op.itemgetter(2),
            reverse=True,
        )
        if not len(edges):
            continue
        for edge in edges[1:]:
            G.remove_edge(*edge[:2])

        try:
            if edges[0][2] <= threshold:  # type: ignore
                G.remove_edge(*edges[0][:2])
        except Exception as E:
            print(edges)
            raise E

    return G


def get_overlap(x: Iterable[Any], y: Iterable[Any]):
    if not isinstance(x, set):
        x = set(x)
    if not isinstance(y, set):
        y = set(y)
    return len(x.intersection(y)) / min(len(x), len(y))


class Assignment(UserList[int]):
    def __init__(self, to: int, over: int, data: Optional[list[int]] = None):
        self.data = data if data is not None else []
        self.to = to
        self.over = over


def divide_overlap(
    G: nx.DiGraph, mesh: Mesh, src_weight: int, rate: float, threshold: float
):
    assigned: list[Assignment] = [
        *it.chain.from_iterable(
            diffusion_division(
                G,
                a,
                b,
                mesh,
                src_weight,
                rate,
                threshold,
            )
            for a, b in G.edges
        )
    ]
    segments: list[list[int]] = []
    for node in G.nodes:
        lost = set().union(*filter(lambda a: a.over == node, assigned))
        current = G.nodes[node]["triangles"]
        segments.append(list(set(current) - set(lost)))
    return segments


def diffusion_division(
    G: nx.DiGraph,
    parent_a: int,
    parent_b: int,
    mesh: Mesh,
    src_weight: int,
    rate: float,
    threshold: float,
):
    print(f"Doing edge ({parent_a}, {parent_b})")
    a_triangles = G.nodes[parent_a]["triangles"]
    b_triangles = G.nodes[parent_b]["triangles"]
    all_triangles = set(a_triangles) | set(b_triangles)
    overlap = set(a_triangles) & set(b_triangles)

    # Precompute edge adjacent triangle neighbors for patch
    neighbors: dict[int, list[int]] = {}
    a_weights: dict[int, float] = {}
    b_weights: dict[int, float] = {}
    for triangle in overlap:
        neighbors[triangle] = mesh.data.get_triangle_neighbors(
            triangle, edge_adjacent=True
        )
        for neighbor in neighbors[triangle]:
            a_weights[neighbor] = 0
            b_weights[neighbor] = 0

            if neighbor not in overlap:
                if neighbor in a_triangles:
                    a_weights[neighbor] = src_weight
                elif neighbor in b_triangles:
                    b_weights[neighbor] = src_weight

    finished = False
    while not finished:
        next_a_weights = copy.copy(a_weights)
        next_b_weights = copy.copy(b_weights)
        above_threshold: list[bool] = []
        for triangle in overlap:
            for curr_weights, next_weights in [
                (a_weights, next_a_weights),
                (b_weights, next_b_weights),
            ]:
                # Calculate influx
                for neighbor in neighbors[triangle]:
                    next_weights[triangle] += curr_weights[neighbor] * rate

                # Calculate eflux (only to adjacent neighbors within the overlap or
                # parent)
                num_neighbors = len(
                    set(neighbors[triangle]) & all_triangles
                )
                next_weights[triangle] -= curr_weights[triangle] * rate * num_neighbors

                # If the new weight is above the threshold, we save a signal to finish
                above_threshold.append(next_weights[triangle] > 0)

        finished = all(above_threshold)
        a_weights |= next_a_weights
        b_weights |= next_b_weights


    # Find final assignments
    assign_a = Assignment(to=parent_a, over=parent_b)
    assign_b = Assignment(to=parent_b, over=parent_a)
    for triangle in overlap:
        if a_weights[triangle] > b_weights[triangle]:
            assign_a.append(triangle)
        elif a_weights[triangle] < b_weights[triangle]:
            assign_b.append(triangle)
        else:
            # If both are equally weighted, decide by coin toss
            random.choice([assign_a, assign_b]).append(triangle)

    return assign_a, assign_b
