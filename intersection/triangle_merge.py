import itertools as it
import more_itertools as itx
from typing import Any, Iterable, NamedTuple, TypeVar
import networkx as nx
from intersection.cortical_intersections import MeshData
import attr


@attr.s(frozen=True, init=False, auto_attribs=True)
class Edge:
    a: int
    b: int

    def __init__(self, a: int, b: int):
        if a < b:
            self.__attrs_init__(a, b)  # type: ignore
        else:
            self.__attrs_init__(b, a)  # type: ignore

    def __eq__(self, other: "Edge"):
        if other.a == self.a:
            if other.b == self.b:
                return True
            return False
        if other.b == self.a:
            if other.a == self.b:
                return True
        return False

    def __contains__(self, item: int):
        if self.a == item or self.b == item:
            return True
        return False

    def __iter__(self):
        return iter([self.a, self.b])

    def other(self, point: int):
        if point == self.a:
            return self.b
        if point == self.b:
            return self.a
        raise ValueError("Point not found in Edge")

    @classmethod
    def mutual_edge(cls, points_a: Iterable[int], points_b: Iterable[int]):
        return cls(*(set(points_a) & set(points_b)))

    @classmethod
    def from_triangle(cls, triangle: int, mesh: MeshData):
        return [cls(a, b) for a, b in it.combinations(mesh.polygons[triangle], 2)]


def merge_triangles(triangles: list[int], mesh: MeshData, radius: int = 2):
    G = nx.Graph()
    G.add_nodes_from(triangles)
    all_triangles = set(triangles)
    visited: dict[int, set[int]] = dict()

    for triangle in triangles:
        blob = triangle_blob(triangle, radius, mesh)
        for neighbor in blob:
            if neighbor not in all_triangles:
                continue

            G.add_edge(neighbor, triangle)
            if triangle not in visited:
                visited[triangle] = blob

    largest_component = max(nx.connected_components(G), key=len)
    if len(largest_component) == 1:
        return mesh.get_triangle_neighbors([*largest_component])
    # Create a set, then list, to remove duplicates
    return [*{*it.chain.from_iterable(visited[x] for x in largest_component)}]


def triangle_blob(core: int, iterations: int, mesh: MeshData):
    result = set()
    blob = [core]
    exclusion = []
    for _ in range(iterations):
        neighbors = mesh.get_triangle_neighbors(blob, exclusion)
        exclusion = blob
        blob = neighbors
        result.update(blob)
    return result


def get_open_ended(patch: list[int], mesh: MeshData):
    patch_set = set(patch)
    open_ended: set[int] = set()
    for triangle in patch:
        for neighbor in mesh.get_triangle_neighbors(triangle):
            if neighbor in patch_set:
                continue
            open_ended.add(triangle)
            break


def fill_holes(patch: list[int], mesh: MeshData):
    patch_set = set(patch)
    open_ended: set[int] = set()
    for triangle in patch:
        for neighbor in mesh.get_triangle_neighbors(triangle):
            if neighbor in patch_set:
                continue
            open_ended.add(triangle)
            break

    open_edges: dict[int, list[Edge]] = {}
    for triangle in open_ended:
        triangle_open_edges: list[Edge] = []
        points = mesh.polygons[triangle]
        for pair in get_adjacent_triangles(points, mesh):
            neighbor = pair.difference([triangle]).pop()
            if neighbor not in patch:
                triangle_open_edges.append(
                    Edge.mutual_edge(mesh.polygons[neighbor], points)
                )
        if triangle_open_edges:
            open_edges[triangle] = triangle_open_edges

    visited: set[Edge] = set()
    loops: list[list[Edge]] = []
    for edge in it.chain.from_iterable(open_edges.values()):
        if edge in visited:
            continue
        loop = find_edge_loop(edge, mesh, open_edges, open_ended)
        visited.update(loop)
        loops.append(loop)

    if len(loops) == 1:
        return

    # The largest loop of edges is the outer perimeter of the patch
    inner_loops = sorted(loops, key=len, reverse=True)[1:]
    for loop in inner_loops:
        patch.extend(get_contained_triangles(loop, patch_set, mesh))


EdgeLoopItems = NamedTuple(
    "EdgeLoopItems",
    [("endpoint", int), ("main_loop", set[Edge]), ("side_loops", list[set[Edge]])],
)


def find_edge_loop(
    start_edge: Edge,
    mesh: MeshData,
    open_edges: dict[int, list[Edge]],
    open_ended_triangles: set[int],
):
    start = start_edge.a
    point = start_edge.b
    ring: list[Edge] = [start_edge]

    loop_limit = 500_000
    i = 0
    current_edge = start_edge

    while point != start:
        if i > loop_limit:
            raise RuntimeError(
                "maximum number of loops reached, and no end to path in site!"
            )
        i += 1
        # loop through open ended triangles next to the point
        candidate_edges: set[Edge] = set()
        for triangle in {
            *mesh.get_triangles_of_point(point)
        } & open_ended_triangles:
            # currently open_ended triangles may have only a single vertex open.
            # so explicitely check if it has a listing in open edges
            if triangle not in open_edges:
                continue
            # find the edge containing "point" and set as the next edge.
            for edge in open_edges[triangle]:
                if edge == current_edge:
                    continue
                if point in edge:
                    candidate_edges.add(edge)

        assert len(candidate_edges)
        if len(candidate_edges) > 1:
            next_edge = None
            edge_chain, triangle_chain = get_radially_sorted_blob(point, mesh)
            supporting_triangle = (
                itx.one(get_adjacent_triangles(current_edge, mesh))
                & open_ended_triangles
            ).pop()
            triangle_i = triangle_chain.index(supporting_triangle)
            edge_i = edge_chain.index(current_edge)
            if triangle_i - edge_i:
                edge_chain = [*reversed(edge_chain)]
                edge_i = len(edge_chain) - edge_i - 1
            for j, edge in enumerate(itx.ncycles(edge_chain, 2)):
                if j <= edge_i:
                    continue
                if edge in candidate_edges:
                    next_edge = edge
                    break
            assert next_edge is not None
        else:
            next_edge = candidate_edges.pop()

        ring.append(next_edge)

        # next_point is the point in next_edge not equal to the current point
        point = next_edge.other(point)
        current_edge = next_edge

    return ring


T = TypeVar("T")
S = TypeVar("S")


def get_radially_sorted_blob(center: int, mesh: MeshData):
    neighbors = mesh.get_triangles_of_point(center)
    triangle_to_edge: dict[int, list[Edge]] = {}
    edge_to_triangle: dict[Edge, list[int]] = {}
    for neighbor in neighbors:
        triangle_to_edge[neighbor] = [
            edge for edge in Edge.from_triangle(neighbor, mesh) if center in edge
        ]
    for edge in it.chain.from_iterable(triangle_to_edge.values()):
        adjacent_triangles: list[int] = []
        for triangle, adjacent_edges in triangle_to_edge.items():
            if edge in adjacent_edges:
                adjacent_triangles.append(triangle)
        edge_to_triangle[edge] = adjacent_triangles

    def step(mapping: dict[T, list[S]], reference: T, previous: S):
        index = mapping[reference].index(previous)
        # index is either one or zero, so flip it using bool
        other = int(not bool(index))
        return mapping[reference][other]

    current_triangle = neighbors[0]
    current_edge = triangle_to_edge[current_triangle][0]
    last_edge = triangle_to_edge[current_triangle][1]
    triangle_chain = [current_triangle]
    # last_edge will be appended as the very last action of the loop
    edge_chain = [current_edge]
    loop_limit = 500
    i = 0
    while current_edge != last_edge:
        if i > loop_limit:
            raise Exception("Looped too many times!")
        i += 1
        current_triangle = step(edge_to_triangle, current_edge, current_triangle)
        triangle_chain.append(current_triangle)
        current_edge = step(triangle_to_edge, current_triangle, current_edge)
        edge_chain.append(current_edge)
    return edge_chain, triangle_chain


def get_contained_triangles(ring: list[Edge], patch: set[int], mesh: MeshData):
    def recurse(ring: set[Edge], contained_triangles: set[int]) -> set[int]:
        open_triangles = {
            *it.chain.from_iterable(
                itx.one(get_adjacent_triangles(edge, mesh))
                - contained_triangles
                - patch
                for edge in ring
            )
        }
        inner_edges: set[Edge] = set()
        for triangle in open_triangles:
            for edge in Edge.from_triangle(triangle, mesh):
                if edge not in ring:
                    inner_edges.add(edge)

        # Check which edges are closed (adjacent to two inner triangles)
        closed_inner_edge: set[Edge] = set()
        for edge in inner_edges:
            # If both adjacent triangles are in open_triangles (so that subtraction
            # yields an empty set), the inner edge is closed
            if not (itx.one(get_adjacent_triangles(edge, mesh)) - open_triangles):
                closed_inner_edge.add(edge)

        contained_triangles.update(open_triangles)
        inner_ring = inner_edges - closed_inner_edge
        if inner_ring:
            return recurse(inner_ring, contained_triangles)
        return contained_triangles

    return recurse(set(ring), set())


def get_adjacent_triangles(points: Iterable[int], mesh: MeshData):
    adj_triangles = [mesh.get_triangles_of_point(point) for point in points]
    for a, b in it.combinations(adj_triangles, 2):
        yield set(a) & set(b)
