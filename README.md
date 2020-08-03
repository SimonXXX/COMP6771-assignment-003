The Testing Regime
==================

The testing code is in 5 files split up according the assignment specification.
The functions have been numbered in the order they appear in that specification document.

* graph_test1.cpp - Constructors
* graph_test2.cpp - Modifiers
* graph_test3.cpp - Accessors and Range Access
* graph_test4.cpp - Comparators, Extractor and Iterators
* graph_test5.cpp - Different types

The last file is a short templated function that can be run on multiple graphs with different types for nodes and weights.
This has been done to further test combinations of types. In the first four file the type have been change between test to 
reduce the possibility of a function only working on a subset of possible type combinations.

The basic data structure of this graph implementation is two sets of shared pointers, one to node objects and the other to edge objects.
Node objects contain the node value and the edge objects the weight and two shared pointers to the source and destination nodes.
In the getters for the edge objects get_count functions are included to check the number of times the pointer is shared. 
This was used during testing to ensure that the pointers were being shared correctly. 
There was no way of putting this in the final test code as there were no public functions in the graph object able to access these getters.
But they did confirm during development that the pointers were shared correctly.


CONSTRUCTORS (spec: section 2.2)
--------------------------------
Four constructors, move constructor/assignment and copy constructor/assignment were tested in graph_test1.
There was extensive 

MODIFIERS (spec: section 2.3)
modifier 1 (inserting a node)
modifier 2 (inserting an edge)
modifier 3 (replacing a node)
modifier 4 (replacing a node and redirect weights to new node)
modifier 5 (erase node and edges from and to that node)
modifier 6 (remove an edge from the graph - with node/node/weight)
modifier 7 (remove an edge from graph - with an iterator)
modifier 8 (erases a range of edges)
modifier 9 (erases all nodes and edges from graph)


ACCESSORS (section 2.4)
accessor 1 (checks if a value represents a node)
accessor 2 (checks if the graph is empty
accessor 3 (checks if two nodes are connected)
accessor 4 (returns a sequence of nodes
accessor 5 (returns a sequence of weights)
accessor 6 (return an iterator to an edge)
accessor 7 (returns a sequence of nodes connected to a given node)

RANGE ACCESS (section 2.5)
range access 1 (return iterator to first element in graph)
range access 2 (return iterator to end of the range of elements

COMPARISONS (section 2.6)
