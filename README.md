This is an application designed to ilustrate how two algorithms (Djikstra and A*) search for the shortest path. The graph is represented as mesh of nodes, each node can be
divided into 4 triangles, the color of each triangle represents the weight of arc from a given neighbour to the node (black color corresponds to non-existing arc). The user 
can see on the right side a large sqaure with 4 green triangles in it. The user can click on each triangle to select it or deselect it (green color indicates selected triangle), 
then if user clicks on some node in the pallet the color inside selected rectangles will change to clicked color. If user has button CHANGE MAP selected, then if he click on some 
node in the grid
the node will be replaced with the big one under the pallet. In this way user can design his own weighted digraph (represented by mesh). If user has selected button SELECT S & T NODE
he can select starting and terminal node in the graph. Based on what algorithm button is selected (either DIJKSTRA ALGORITHM or A* ALGORITHM) programm will search for the
shortest path from starting node to terminal after pressing ENTER. Nodes that are curently in the queue are marked by light green color, with pink color the currently searched 
node and by blue 
color will be marked nodes of shortest path. On each node will be written shortest distance from starting node that given algorithm at current step
knows of, if A* is selected then by g* we mean 
current shortest distance from starting node and by f* we mean heuristic (in this case we are using Manhattan distance).
