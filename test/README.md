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
This has been done to further test combinations of types. In the first four file the type have been changed between testd to 
reduce the possibility of a function only working on a subset of possible type combinations. It was not practical to test all possible combinations
of all eligible types.

The basic data structure of this graph implementation is two sets of shared pointers, one to node objects and the other to edge objects.
Node objects contain the node value and the edge objects the weight and two shared pointers to the source and destination nodes.
In the getters for the edge objects get_count functions are included to check the number of times the pointer is shared. 
This was used during testing to ensure that the pointers were being shared correctly. 
There was no way of putting this in the final test code as there were no public functions in the graph object able to access these getters.
But they did confirm during development that the pointers were shared correctly.


CONSTRUCTORS (section 2.2)
--------------------------------
Four constructors, move constructor/assignment and copy constructor/assignment were tested in graph_test1.
There was extensive testing here with different type combinations and empty vectors and initializer lists.
Each constructor or assignment operator has its own test case with sections dealing with different set ups.



MODIFIERS (section 2.3)
-----------------------------
The modifiers,i.e. ways of inserting, replacing, removing or erasing nodes and edges were tested in graph_test2.
The modifiers were numbered according to their order in the assignment specification.
All cases that would throw exceptions were tested.
There was extensive testing here with different types and combinations of graphs that were empty, node only and fully populated with nodes and edges.


ACCESSORS (section 2.4)
-----------------------
The accessors,i.e. ways of getting data from the graph and check if the graph contained specific data were tested in graph_test3.
There was less extensive testing here as many of the functions had been extensively used in the previous two test files.
All cases that would throw exceptions were tested. 
There was suficient testing with different types and combinations of graphs that were empty, node only and fully populated with nodes and edges.


RANGE ACCESS (section 2.5)
--------------------------
The range access (i.e. begin() and end()) functions were also tested in graph_test3.
Although they have been tested before and will be checked again in the iterator testing.


COMPARISONS (section 2.6) 
-------------------------
The equality operator was tested in graph_test4 both on populated and empty graphs.
Although it has been tested in previous test files.


EXTRACTOR (section 2.7
----------------------
The formatted output function was also tested in graph_test4 both on populated and empty graphs.
Although it has been extensively used in previous test files.

ITERATOR (section 2.8
---------------------
The iterator functionality was formally tested in graph_test4.
Although it has been extensively used in previous test files.

graph_test5
-----------
This file contains a templated function to test various combinations of node and edge types.
This proved very useful in development of the graph application and hightlighted some errors in the code.
However, this could do with some more development to allow more testing of the graph functions across different type combinations.
It 











