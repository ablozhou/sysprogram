import networkx as nx
import matplotlib.pyplot as plt
import random

def create_graph(n, m):
    # Create an empty graph
    G = nx.Graph()

    # Add n vertices to the graph
    for i in range(n):
        G.add_node(i)

    # Add m edges to the graph
    edges_added = 0
    while edges_added < m:
        # Generate a random edge
        u = random.randint(0, n-1)
        v = random.randint(0, n-1)

        # Ensure no self-loops and no duplicate edges
        if u != v and not G.has_edge(u, v):
            G.add_edge(u, v)
            edges_added += 1

    return G

def visualize_graph(G):
    # Draw the graph
    pos = nx.spring_layout(G)
    nx.draw(G, pos, with_labels=True, node_color='skyblue', node_size=700, edge_color='gray', font_size=15)
    plt.title("Graph with {} vertices and {} edges".format(len(G.nodes), len(G.edges)))
    plt.show()

# Input parameters
n = 10  # Number of vertices
m = 15  # Number of edges

# Create and visualize the graph
G = create_graph(n, m)
visualize_graph(G)
