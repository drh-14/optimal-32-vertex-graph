typedef struct Graph;
Graph build_graph(int num_vertices, int num_edges, int **edge_list);
void delete_graph(Graph g);
int *shortest_path(int source, Graph g);
int average_shortest_path_length(Graph g);