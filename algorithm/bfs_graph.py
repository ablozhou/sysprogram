import matplotlib.pyplot as plt
from typing import List, Dict
import networkx as nx
import sys

class EdgeNode:
    def __init__(self, x, y, weight, direct):
        self.x = x
        self.y = y
        self.weight = weight
        self.direct = direct
        
        
class Graph:
    def __init__(self, nEdges):
        
        self.nEdges = nEdges
        self.edge_nodes = {}
        
    def build(self):
        for i in range(self.nEdges):
            #[x,y,weight,direct] = data.split(",")
            self.inputEdgeNode()
            # fibnacii, dfs/bfs
    def inputEdgeNode(self):
        x=0
        y=0
        weight=0
        direct=0
        data=input("\nInput: \nx y [w] [d]:\n")
        input_array = [x for x in data.split()]
        if len(input_array) == 2:
            [x,y]= input_array
        elif len(input_array) == 3:
            [x,y,weight] = input_array
            weight = int(weight)
        elif len(input_array) == 4:
            [x,y,weight,direct] = input_array
            weight = int(weight)
            direct = int(direct)
        else:
            print("Error input.\n")
            exit(1)
        
        en = EdgeNode(x,y,weight,direct)
        self.addEdgeNode(en)
        
    def add_nodes(self, nodes:List[EdgeNode]):
        for node in nodes:
            self.addEdgeNode(node)
            
    def addEdgeNode(self,en:EdgeNode):
        
        if not en.direct:
            self.addEdgeNode(EdgeNode(en.y,en.x,en.weight,1))
            en.direct = 1
            self.addEdgeNode(en)
        else:
            if en.x in self.edge_nodes:
                self.edge_nodes[en.x].append(en)
            else:
                self.edge_nodes[en.x]=[en]
            if not en.y in self.edge_nodes:
                self.edge_nodes[en.y]=[]
            
    def print(self):
        for node, edges in self.edge_nodes.items():
            for edge in edges:
                direction = "->" if edge.direct else "<->"
                print(f"{edge.x} {direction} {edge.y} (weight: {edge.weight})")

    def draw(self):
        G = nx.DiGraph()
        for node, edges in self.edge_nodes.items():
            for edge in edges:
                G.add_edge(edge.x, edge.y, weight=edge.weight)

        pos = nx.spring_layout(G)
        edge_labels = {(edge.x, edge.y): edge.weight for edges in self.edge_nodes.values() for edge in edges}
        nx.draw(G, pos, with_labels=True, node_color='skyblue', node_size=700, edge_color='gray', font_size=15)
        nx.draw_networkx_edge_labels(G, pos, edge_labels=edge_labels)
        plt.show()

# Example usage

        
        
def main():
    #n = 3  # Number of vertices
    m = 2  # Number of edges
    if len(sys.argv) == 2:
        #n = int(sys.argv[1]) # Number of vertices
        m = int(sys.argv[1]) # Number of edges
    else:
        print(f'Usage: {sys.argv[0]} [vertices] [edges]')
        exit(1)
    print("please input data of edge nodes. ")
    print("\n\t x: parent node index. \n\t y: subnode index, \n\t w: edge weight, \n\t d: direction. 0 no direction,1 direction from x to y.\n")
    graph = Graph(m)
    graph.build()
    graph.print()
    graph.draw()
    

if __name__ == "__main__":
    main()
        
        