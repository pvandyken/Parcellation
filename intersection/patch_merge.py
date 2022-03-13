from copy import copy
import itertools as it
import more_itertools as itx
import operator as op
from typing import Any, Iterable

import networkx as nx


def merge_overlapping(G: nx.DiGraph, threshold: int) -> tuple[set[int], ...]:


    def recurse(root: int) -> tuple[set[int], ...]:
        root_triangles: set[int] = set(G.nodes[root]["triangles"])
        all_triangles = copy(root_triangles)
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


def get_overlap_graph(
    fronts: list[list[int]], backs: list[list[int]], threshold: float
):
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
            reverse=True,
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


def get_overlap(x: Iterable[Any], y: Iterable[Any]):
    if not isinstance(x, set):
        x = set(x)
    if not isinstance(y, set):
        y = set(y)
    return len(x.intersection(y)) / min(len(x), len(y))
