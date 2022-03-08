import itertools as it
import networkx as nx
from intersection.mesh import Mesh

def merge_triangles(triangles: list[int], mesh: Mesh, radius: int = 2):
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

    largest_component = max(nx.connected_components(G))
    if len(largest_component) == 1:
        return mesh.data.get_triangle_neighbors([*largest_component])
    return [*it.chain.from_iterable(visited[x] for x in largest_component)]


def triangle_blob(core: int, iterations: int, mesh: Mesh):
    result = set()
    blob = [core]
    exclusion = []
    for _ in range(iterations):
        neighbors = mesh.data.get_triangle_neighbors(blob, exclusion)
        exclusion = blob
        blob = neighbors
        result.update(blob)
    return result
