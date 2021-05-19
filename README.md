# DIJKSTRA DISTANCE

  We essentially build a tree.  We start with only node 0 connected
  to the tree, and this is indicated by setting CONNECTED[0] = 1.

  We initialize MIND[I] to the one step distance from node 0 to node I.

  Now we search among the unconnected nodes for the node MV whose minimum
  distance is smallest, and connect it to the tree.  For each remaining
  unconnected node I, we check to see whether the distance from 0 to MV
  to I is less than that recorded in MIND[I], and if so, we can reduce
  the distance.

  After NV-1 steps, we have connected all the nodes to 0, and computed
  the correct minimum distances.

  Parameters:

  Input, int OHD[NV][NV], the distance of the direct link between
  nodes I and J.

  Output, int *MIND[NV], the minimum distance from node 0 to each node.
