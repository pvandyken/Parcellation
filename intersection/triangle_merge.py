import itertools as it
import networkx as nx
from intersection.mesh import Mesh

def merge_triangles(triangles: list[int], mesh: Mesh):
    G = nx.Graph()
    G.add_nodes_from(triangles)

    for a, b in it.combinations(triangles, 2):
        neighbors_a = mesh.data.get_triangle_neighbors(a)
        if b in neighbors_a:
            G.add_edge(a, b)
            continue
        next_neighbors = set()
        for neighbor in neighbors_a:
            for sub_neighbor in mesh.data.get_triangle_neighbors(neighbor):
                next_neighbors.add(sub_neighbor)
        if b in next_neighbors:
            G.add_edge(a, b)

    largest_component = G.subgraph(max(nx.connected_components(G)))
    return [*it.chain(mesh.data.get_triangle_neighbors(x) for x in largest_component)]
