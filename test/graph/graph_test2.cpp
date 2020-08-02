#include "gdwg/graph.hpp"

#include <catch2/catch.hpp>
#include <concepts/concepts.hpp>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <linux/limits.h>
#include <sstream>
// =============================
// MODIFIERS (spec: section 2.3)
// -----------------------------
TEST_CASE("modifier 1 (inserting a node)") {
	SECTION("insert node in graph") {
		using graph = gdwg::graph<double, char>;
		// insert in empty graph
		auto g1 = graph{};
		REQUIRE(g1.insert_node(5.42));
		CHECK(!g1.insert_node(5.42));
		// insert in populated graph
		auto v = std::vector<graph::value_type>{{12.12, 99.9, '4'}, {1.1, 2.2, '3'}, {7.7, 7.7, 'a'}};
		auto g2 = graph(v.begin(), v.end());
		auto dbl_var = 999.999;
		REQUIRE(g1.insert_node(dbl_var));
		CHECK(!g1.insert_node(dbl_var));
	}
	SECTION("insert empty string node in graph") {
		using graph1 = gdwg::graph<std::string, int>;
		auto g1 = graph1();
		CHECK(g1.insert_node(""));
		CHECK(!g1.insert_node(""));
	}
}
TEST_CASE("modifier 2 (inserting an edge)") {
	SECTION("insert edges in empty graph") {
		using graph = gdwg::graph<double, char>;
		// insert in empty graph
		auto g1 = graph{};
		CHECK_THROWS_WITH(g1.insert_edge(5.42, 42000.00042, '+'),
		                  "Cannot call gdwg::graph<N, E>::insert_edge when either src or dst node "
		                  "does not exist");
		REQUIRE(g1.insert_node(5.42));
		CHECK_THROWS_WITH(g1.insert_edge(5.42, 42000.00042, '+'),
		                  "Cannot call gdwg::graph<N, E>::insert_edge when either src or dst node "
		                  "does not exist");
		REQUIRE(g1.insert_node(42000.00042));
		CHECK(g1.insert_edge(5.42, 42000.00042, '+'));
		CHECK(!g1.insert_edge(5.42, 42000.00042, '+'));
	}
	SECTION("insert edges populated in graph - nodes exist but have no edges") {
		using graph = gdwg::graph<double, char>;
		auto v = std::vector<graph::value_type>{{12.12, 99.9, '4'}, {1.1, 2.2, '3'}, {7.7, 7.7, 'a'}};
		auto g1 = graph(v.begin(), v.end());
		REQUIRE(g1.insert_node(999.999));
		REQUIRE(g1.insert_edge(999.999, 999.999, ' '));
		CHECK(!g1.insert_edge(999.999, 999.999, ' '));
		REQUIRE(g1.insert_node(181.818));
		CHECK(g1.insert_edge(181.818, 2.2, '='));
		CHECK(!g1.insert_edge(181.818, 2.2, '='));
	}
	SECTION("insert empty string node and edge in graph") {
		using graph1 = gdwg::graph<std::string, char>;
		auto g1 = graph1();
		auto char_var = '\u0000';
		REQUIRE(g1.insert_node(""));
		CHECK(g1.insert_edge("", "", char_var));
		CHECK(!g1.insert_edge("", "", char_var));
	}
}

TEST_CASE("modifier 3 (replacing a node)") {
	SECTION("replace node in populated graph") {
		using graph = gdwg::graph<double, std::string>;
		auto v = std::vector<graph::value_type>{{12.12, 99.9, "4"}, {1.1, 2.2, "3"}, {7.7, 7.7, "a"}};
		auto g1 = graph(v.begin(), v.end());
		CHECK_THROWS_WITH(g1.replace_node(999.0, 21.12), // old node doesnt exist
		                  "Cannot call gdwg::graph<N, E>::replace_node on a node that doesn't exist");
		CHECK_NOTHROW(g1.replace_node(12.12, 42.42)); // new node doesnt exist
		REQUIRE(g1.is_node(42.42));
		CHECK(!g1.insert_edge(42.42, 99.9, "4"));
	}
	SECTION("insert edges in one node graph") {
		using graph = gdwg::graph<int, double>;
		auto g1 = graph();
		g1.insert_node(55);
		REQUIRE(g1.replace_node(55, -55));
		CHECK(g1.is_node(-55));
		CHECK(!g1.is_node(55));
		// put an edge in to and from this node and test again
		g1.insert_edge(-55, -55, 11.11);
		REQUIRE(g1.replace_node(-55, 333));
		CHECK(g1.is_node(333));
		CHECK(!g1.is_node(-55));
		CHECK(g1.is_connected(333, 333));
	}
	SECTION("insert empty string node and edge in graph") {
		using graph = gdwg::graph<int, double>;
		auto g1 = graph();
		CHECK_THROWS_WITH(g1.replace_node(12, 21), // old node doesnt exist
		                  "Cannot call gdwg::graph<N, E>::replace_node on a node that doesn't exist");
	}
}
TEST_CASE("modifier 4 (replacing a node and redirect weights to new node)") {
	SECTION("Example given in spec") {
		// 	Operation: merge_replace_node(A, B)
		// Graph before: (A,B,1),(A,C,2),(A,D,3)
		// Graph after : (B,B,1),(B,C,2),(B,D,3)
		auto g1 = gdwg::graph<char, int>{};
		g1.insert_node('A');
		g1.insert_node('B');
		g1.insert_node('C');
		g1.insert_node('D');
		g1.insert_edge('A', 'B', 1);
		g1.insert_edge('A', 'C', 2);
		g1.insert_edge('A', 'D', 3);
		g1.merge_replace_node('A', 'B');

		auto out = std::ostringstream{};
		out << g1;
		auto const expected_output = std::string_view(R"(B (
  B | 1
  C | 2
  D | 3
)
C (
)
D (
)
)");
		CHECK(out.str() == expected_output);
	}
	SECTION("Graph with all edges going to same node") {
		// 	Operation: merge_replace_node(1, 2)
		// Graph before: (1,1,'a'), (2,2,'a'), (3,3,'a')
		// Graph after : (2,2,'a'), (3,3,'a')
		using graph = gdwg::graph<int, char>;
		auto v = std::vector<graph::value_type>{{1, 1, 'a'}, {2, 2, 'a'}, {3, 3, 'a'}};
		auto g1 = graph(v.begin(), v.end());
		g1.merge_replace_node(1, 2);
		auto out = std::ostringstream{};
		out << g1;
		auto const expected_output = std::string_view(R"(2 (
  2 | a
)
3 (
  3 | a
)
)");
		CHECK(out.str() == expected_output);
	}
	SECTION("Graph with two nodes and no edges") {
		// 	Operation: merge_replace_node(1, 2)
		using graph = gdwg::graph<int, char>;
		auto g1 = graph{};
		g1.insert_node(1);
		g1.insert_node(2);
		g1.merge_replace_node(1, 2);
		auto out = std::ostringstream{};
		out << g1;
		auto const expected_output = std::string_view(R"(2 (
)
)");
		CHECK(out.str() == expected_output);
	}
	SECTION("Merging two nodes with no edges - graph has other edges connected to other nodes") {
		// 	Operation: merge_replace_node(55, 66)
		using graph = gdwg::graph<int, char>;
		auto v = std::vector<graph::value_type>{{1, 1, 'a'}, {2, 2, 'a'}, {3, 3, 'a'}};
		auto g1 = graph(v.begin(), v.end());
		g1.insert_node(55);
		g1.insert_node(66);
		g1.merge_replace_node(55, 66);
		auto out = std::ostringstream{};
		out << g1;
		auto const expected_output = std::string_view(R"(1 (
  1 | a
)
2 (
  2 | a
)
3 (
  3 | a
)
66 (
)
)");
		CHECK(out.str() == expected_output);
	}
	SECTION("with data that is incorrect") {
		// Graph : (A,B,1),(A,C,2),(A,D,3),(C,D,4)
		using graph = gdwg::graph<char, int>;
		auto v =
		   std::vector<graph::value_type>{{'A', 'B', 1}, {'A', 'C', 2}, {'A', 'D', 3}, {'C', 'D', 4}};
		auto g1 = graph(v.begin(), v.end());
		CHECK(!g1.erase_edge('C', 'D', 555));
		CHECK_THROWS_WITH(g1.merge_replace_node('E', 'D'), // scr node doesnt exist
		                  "Cannot call gdwg::graph<N, E>::merge_replace_node on old or new data if "
		                  "they don't exist in the graph");
		CHECK_THROWS_WITH(g1.merge_replace_node('C', 'E'), // dst node doesnt exist
		                  "Cannot call gdwg::graph<N, E>::merge_replace_node on old or new data if "
		                  "they don't exist in the graph");
		CHECK(g1.is_node('C'));
		CHECK(g1.is_node('D'));
		CHECK(!g1.weights('C', 'D').empty());
	}
}

TEST_CASE("modifier 5 (erase node and edges from and to that node)") {
	SECTION("all edges go to one node which is removed") {
		// Graph before: (A,B,1),(A,C,2),(A,D,3)
		// Graph after : no edges
		using graph = gdwg::graph<char, int>;
		auto v = std::vector<graph::value_type>{{'A', 'B', 1}, {'A', 'C', 2}, {'A', 'D', 3}};
		auto g1 = graph(v.begin(), v.end());
		REQUIRE(g1.erase_node('A'));
		CHECK(!g1.is_node('A'));
		CHECK(ranges::all_of(g1.nodes(), [&](char node) { return g1.connections(node).empty(); }));
	}
	SECTION("node has no edges") {
		// Graph before: (A,B,1),(A,C,2),(A,D,3) plus node E
		// Graph after : no edges
		using graph = gdwg::graph<char, int>;
		auto v = std::vector<graph::value_type>{{'A', 'B', 1}, {'A', 'C', 2}, {'A', 'D', 3}};
		auto g1 = graph(v.begin(), v.end());
		REQUIRE(g1.insert_node('E'));
		auto g2(g1); // make a copy to compare later
		REQUIRE(g1.erase_node('E'));
		CHECK(!g1.is_node('E'));
		REQUIRE(g1.insert_node('E')); // put the node back
		CHECK(g1 == g2);
	}
}

TEST_CASE("modifier 6 (remove an edge from the graph - with node/node/weight)") {
	SECTION("erase edge to node with multiple edges") {
		// Graph before: (A,B,1),(A,C,2),(A,D,3)
		// Graph after : (A,C,2),(A,D,3) plus node B
		using graph = gdwg::graph<char, int>;
		auto v = std::vector<graph::value_type>{{'A', 'B', 1}, {'A', 'C', 2}, {'A', 'D', 3}};
		auto g1 = graph(v.begin(), v.end());
		REQUIRE(g1.erase_edge('A', 'B', 1));
		CHECK(g1.is_node('A'));
		CHECK(g1.is_node('B'));
		CHECK(g1.weights('A', 'B').empty());
	}
	SECTION("erase edge to node with one edge") {
		// Graph before: (A,B,1),(A,C,2),(A,D,3),(C,D,4)
		// Graph after : (A,B,1),(A,C,2),(A,D,3) plus node C
		using graph = gdwg::graph<char, int>;
		auto v =
		   std::vector<graph::value_type>{{'A', 'B', 1}, {'A', 'C', 2}, {'A', 'D', 3}, {'C', 'D', 4}};
		auto g1 = graph(v.begin(), v.end());
		REQUIRE(g1.erase_edge('C', 'D', 4));
		CHECK(g1.is_node('C'));
		CHECK(g1.is_node('D'));
		CHECK(g1.weights('C', 'D').empty());
	}
	SECTION("erase edge with src and dst the same node") {
		// Graph before: (A,B,1),(A,C,2),(A,D,3),(D,D,4)
		// Graph after : (A,B,1),(A,C,2),(A,D,3)
		using graph = gdwg::graph<char, int>;
		auto v =
		   std::vector<graph::value_type>{{'A', 'B', 1}, {'A', 'C', 2}, {'A', 'D', 3}, {'D', 'D', 4}};
		auto g1 = graph(v.begin(), v.end());
		REQUIRE(g1.erase_edge('D', 'D', 4));
		CHECK(g1.is_node('C'));
		CHECK(g1.is_node('D'));
		CHECK(g1.weights('D', 'D').empty());
	}
	SECTION("with data that is incorrect") {
		// Graph : (A,B,1),(A,C,2),(A,D,3),(C,D,4)
		using graph = gdwg::graph<char, int>;
		auto v =
		   std::vector<graph::value_type>{{'A', 'B', 1}, {'A', 'C', 2}, {'A', 'D', 3}, {'C', 'D', 4}};
		auto g1 = graph(v.begin(), v.end());
		CHECK(!g1.erase_edge('C', 'D', 555));
		CHECK_THROWS_WITH(g1.erase_edge('E', 'D', 4), // scr node doesnt exist
		                  "Cannot call gdwg::graph<N, E>::erase_edge on src or dst if they don't "
		                  "exist in the graph");
		CHECK_THROWS_WITH(g1.erase_edge('C', 'E', 4), // dst node doesnt exist
		                  "Cannot call gdwg::graph<N, E>::erase_edge on src or dst if they don't "
		                  "exist in the graph");
		CHECK_THROWS_WITH(g1.erase_edge('E', 'F', 4), // both nodes dont exist
		                  "Cannot call gdwg::graph<N, E>::erase_edge on src or dst if they don't "
		                  "exist in the graph");
		CHECK(g1.is_node('C'));
		CHECK(g1.is_node('D'));
		CHECK(!g1.weights('C', 'D').empty());
	}
}

TEST_CASE("modifier 7 (remove an edge from graph - with an iterator)") {
	SECTION("erase edge to node with multiple edges") {
		// Graph before: (A,B,1),(A,C,2),(A,D,3)
		// Graph after : (A,C,2),(A,D,3) plus node B
		using graph = gdwg::graph<char, int>;
		auto v = std::vector<graph::value_type>{{'A', 'B', 1}, {'A', 'C', 2}, {'A', 'D', 3}};
		auto g1 = graph(v.begin(), v.end());
		auto it = g1.find('A', 'B', 1);
		auto return_it = g1.erase_edge(it);
		CHECK(g1.is_node('A'));
		CHECK(g1.is_node('B'));
		CHECK(g1.weights('A', 'B').empty());
		CHECK(std::get<0>(*return_it) == 'A');
		CHECK(std::get<1>(*return_it) == 'C');
		CHECK(std::get<2>(*return_it) == 2);
	}
	SECTION("erase edge to node with one edge") {
		// Graph before: (A,B,1),(A,C,2),(A,D,3),(C,D,4)
		// Graph after : (A,B,1),(A,C,2),(A,D,3) plus node C
		using graph = gdwg::graph<char, int>;
		auto v =
		   std::vector<graph::value_type>{{'A', 'B', 1}, {'A', 'C', 2}, {'A', 'D', 3}, {'C', 'D', 4}};
		auto g1 = graph(v.begin(), v.end());
		auto it = g1.find('C', 'D', 4);
		auto return_it = g1.erase_edge(it);
		CHECK(g1.is_node('C'));
		CHECK(g1.is_node('D'));
		CHECK(g1.weights('C', 'D').empty());
		CHECK(return_it == g1.end()); // the last edge was erased
	}
	SECTION("erase edge with src and dst the same node") {
		// Graph before: (A,B,1),(A,C,2),(A,D,3),(D,D,4)
		// Graph after : (A,B,1),(A,C,2),(A,D,3)
		using graph = gdwg::graph<char, int>;
		auto v =
		   std::vector<graph::value_type>{{'A', 'B', 1}, {'A', 'C', 2}, {'A', 'D', 3}, {'D', 'D', 4}};
		auto g1 = graph(v.begin(), v.end());
		auto it = g1.find('D', 'D', 4);
		auto return_it = g1.erase_edge(it);
		CHECK(g1.is_node('D'));
		CHECK(g1.weights('D', 'D').empty());
		CHECK(return_it == g1.end()); // the last edge was erased
	}
}
TEST_CASE("modifier 8 (erases a range of edges)") {
	SECTION("erase three edges from middle of graph") {
		using graph = gdwg::graph<int, int>;
		auto const v = std::vector<graph::value_type>{
		   {4, 1, -4}, // this will not erased and will be pointed to by the returned iterator
		   {3, 2, 2}, // this will be erased
		   {2, 4, 2}, // this will be erased
		   {2, 1, 1}, // this will be erased
		   {6, 2, 5},
		   {6, 3, 10},
		   {1, 5, -1},
		   {3, 6, -8}, // this will be erased
		   {4, 5, 3},
		   {5, 2, 7},
		};
		auto g1 = graph(v.begin(), v.end());
		auto i = g1.find(2, 4, 2);
		auto s = g1.find(4, 1, -4);
		auto return_it = g1.erase_edge(i, s);
		CHECK(std::get<0>(*return_it) == 4);
		CHECK(std::get<1>(*return_it) == 1);
		CHECK(std::get<2>(*return_it) == -4);
		CHECK(g1.is_connected(1, 5));
		CHECK(g1.is_connected(4, 1));
		CHECK(g1.is_connected(4, 5));
		CHECK(g1.is_connected(5, 2));
		CHECK(g1.is_connected(6, 2));
		CHECK(g1.is_connected(6, 3));
	}
	SECTION("erase two edges from end of graph") {
		using graph = gdwg::graph<int, int>;
		auto const v = std::vector<graph::value_type>{{4, 1, -4}, {5, 2, 7}, {6, 2, 5}, {6, 3, 10}};
		auto g1 = graph(v.begin(), v.end());
		auto i = g1.find(6, 2, 5);
		auto s = g1.end();
		auto return_it = g1.erase_edge(i, s);
		CHECK(return_it == g1.end());
		CHECK(g1.is_connected(4, 1));
		CHECK(g1.is_connected(5, 2));
	}
	SECTION("erase one edges from beginning of graph") {
		using graph = gdwg::graph<int, int>;
		auto const v = std::vector<graph::value_type>{{4, 1, -4}, {5, 2, 7}, {6, 2, 5}, {6, 3, 10}};
		auto g1 = graph(v.begin(), v.end());
		auto i = g1.find(4, 1, -4);
		auto s = g1.find(5, 2, 7);
		auto return_it = g1.erase_edge(i, s);
		CHECK(std::get<0>(*return_it) == 5);
		CHECK(std::get<1>(*return_it) == 2);
		CHECK(std::get<2>(*return_it) == 7);
		CHECK(g1.is_connected(5, 2));
		CHECK(g1.is_connected(6, 2));
		CHECK(g1.is_connected(6, 3));
	}
	SECTION("set one or both parameters to end() - i and s the same value") {
		using graph = gdwg::graph<int, int>;
		auto const v = std::vector<graph::value_type>{{4, 1, -4}, {5, 2, 7}, {6, 2, 5}, {6, 3, 10}};
		auto g1 = graph(v.begin(), v.end());
		auto g2 = g1;
		auto i = g1.end();
		auto s = g1.find(5, 2, 7);
		auto return_it = g1.erase_edge(i, s);
		CHECK(g1 == g2);
		CHECK(return_it == g1.end());
		auto i1 = g1.end();
		auto s1 = g1.end();
		auto return_it1 = g1.erase_edge(i1, s1);
		CHECK(g1 == g2);
		CHECK(return_it1 == g1.end());
		auto i2 = g1.find(5, 2, 7);
		auto s2 = g1.find(5, 2, 7);
		auto return_it2 = g1.erase_edge(i2, s2);
		CHECK(g1 == g2);
		CHECK(return_it2 == s2);
	}
}
TEST_CASE("modifier 9 (erases all nodes and edges from graph)") {
	SECTION("clear a graph and check there are no nodes") {
		using graph = gdwg::graph<char, int>;
		auto v = std::vector<graph::value_type>{{'A', 'B', 1}, {'A', 'C', 2}, {'A', 'D', 3}};
		auto g1 = graph(v.begin(), v.end());
		REQUIRE(g1.insert_node('E'));
		auto g2 = g1;
		g1.clear();
		CHECK(g1 != g2);
		CHECK(!g1.is_node('A'));
		CHECK(!g1.is_node('B'));
		CHECK(!g1.is_node('C'));
		CHECK(!g1.is_node('D'));
		CHECK(!g1.is_node('E'));
	}
}

// TEST_CASE("2.7 Extractor") {
// 	using graph = gdwg::graph<int, int>;
// 	auto const v = std::vector<graph::value_type>{
// 	   {4, 1, -4},
// 	   {3, 2, 2},
// 	   {2, 4, 2},
// 	   {2, 1, 1},
// 	   {6, 2, 5},
// 	   {6, 3, 10},
// 	   {1, 5, -1},
// 	   {3, 6, -8},
// 	   {4, 5, 3},
// 	   {5, 2, 7},
// 	};

// 	auto g = graph(v.begin(), v.end());
// 	g.insert_node(64);
// 	auto out = std::ostringstream{};
// 	out << g;
// 	auto const expected_output = std::string_view(R"(1 (
//   5 | -1
// )
// 2 (
//   1 | 1
//   4 | 2
// )
// 3 (
//   2 | 2
//   6 | -8
// )
// 4 (
//   1 | -4
//   5 | 3
// )
// 5 (
//   2

// TEST_CASE("2.7 Extractor") {
// 	using graph = gdwg::graph<int, int>;
// 	auto const v = std::vector<graph::value_type>{
// 	   {4, 1, -4},
// 	   {3, 2, 2},
// 	   {2, 4, 2},
// 	   {2, 1, 1},
// 	   {6, 2, 5},
// 	   {6, 3, 10},
// 	   {1, 5, -1},
// 	   {3, 6, -8},
// 	   {4, 5, 3},
// 	   {5, 2, 7},
// 	};

// TEST_CASE("2.7 Extractor") {
// 	using graph = gdwg::graph<int, int>;
// 	auto const v = std::vector<graph::value_type>{
// 	   {4, 1, -4},
// 	   {3, 2, 2},
// 	   {2, 4, 2},
// 	   {2, 1, 1},
// 	   {6, 2, 5},
// 	   {6, 3, 10},
// 	   {1, 5, -1},
// 	   {3, 6, -8},
// 	   {4, 5, 3},
// 	   {5, 2, 7},
// 	};

// 	auto g = graph(v.begin(), v.end());
// 	g.insert_node(64);
// 	auto out = std::ostringstream{};
// 	out << g;
// 	auto const expected_output = std::string_view(R"(1 (
//   5 | -1
// )
// 2 (
//   1 | 1
//   4 | 2
// )
// 3 (
//   2 | 2
//   6 | -8
// )
// 4 (
//   1 | -4
//   5 | 3
// )
// 5 (
//   2 | 7
// )
// 6 (uto out = std::ostringstream{};
// 	out << g;
// 	auto const expected_output = std::string_view(R"(1 (
//   5 | -1
// )
// 2 (
//   1 | 1
//   4 | 2
// )
// 3 (
//   2 | 2
//   6 | -8
// )
// 4 (
//   1 | -4
//   5 | 3
// )
// 5 (
//   2 | 7
// )
// 6 ( | 7
// )
// 6 (
//   2 | 5
//   3 | 10
// )
// 64 (
// )
// )");
// 	CHECK(out.str() == expected_output);
// 	// g.print_edges();
// }

// 	SECTION("is_connected") {
// 		auto v = std::vector<char>{'A', 'B', 'C', 'D', 'a', 'b', 'c'};
// 		auto g1 = gdwg::graph<char, int>(v.begin(), v.end());
// 		g1.insert_edge('A', 'B', 1);
// 		g1.insert_edge('A', 'C', 2);
// 		g1.insert_edge('A', 'D', 3);
// 		g1.insert_edge('D', 'C', 22);
// 		CHECK(!g1.is_connected('a', 'b')); // neither nodes connected to anything
// 		g1.insert_edge('a', 'C', 22);
// 		CHECK(!g1.is_connected('a', 'b')); // first node connected, second no edges
// 		CHECK(!g1.is_connected('c', 'a')); // second node connected, first no edges
// 		g1.insert_edge('b', 'C', 22);
// 		CHECK(!g1.is_connected('a', 'b')); // both nodes connected but not to each other
// 		CHECK(g1.is_connected('D', 'C')); // nodes connected to each other
// 		CHECK_THROWS_WITH(g1.is_connected('A', 'E'),
// 		                  "Cannot call gdwg::graph<N, E>::is_connected if src or dst node "
// 		                  "don't "
// 		                  "exist in the graph");
// 		CHECK_THROWS_WITH(g1.is_connected('E', 'A'),
// 		                  "Cannot call gdwg::graph<N, E>::is_connected if src or dst node "
// 		                  "don't "
// 		                  "exist in the graph");
// 		for (auto i : g1.nodes()) {
// 			std::cout << i << " ";
// 		}
// 		g1.insert_edge('A', 'C', 12);
// 		g1.insert_edge('A', 'C', 22);
// 		g1.insert_edge('A', 'C', 32);
// 		g1.insert_edge('A', 'C', 42);
// 		g1.insert_edge('A', 'C', -12121212);
// 		std::cout << std::endl;

// 		for (auto i : g1.weights('A', 'C')) {
// 			std::cout << i << " ";
// 		}
// 		std::cout << std::endl;
// 	}
// 	SECTION("weights") {
// 		auto v = std::vector<char>{0, 1, 4, 5, 6, 7, 2, 3};
// 		auto g1 = gdwg::graph<int, std::string>(v.begin(), v.end());
// 		g1.insert_edge(1, 4, "hello");
// 		g1.insert_edge(1, 5, "1hello");
// 		g1.insert_edge(1, 7, "2hello");
// 		g1.insert_edge(1, 6, "3hello");
// 		g1.insert_edge(1, 6, "7hello");
// 		g1.insert_edge(0, 7, "34");
// 		g1.insert_edge(1, 6, "!");
// 		CHECK_THROWS_WITH(g1.is_connected('A', 'E'),
// 		                  "Cannot call gdwg::graph<N, E>::is_connected if src or dst node "
// 		                  "don't "
// 		                  "exist in the graph");
// 		CHECK_THROWS_WITH(g1.is_connected('E', 'A'),
// 		                  "Cannot call gdwg::graph<N, E>::is_connected if src or dst node "
// 		                  "don't "
// 		                  "exist in the graph");

// 		for (const auto& i : g1.weights(1, 6)) {
// 			std::cout << i << " ";
// 		}
// 		std::cout << std::endl;
// 		for (const auto& i : g1.connections(1)) {
// 			std::cout << i << " ";
// 		}
// 		std::cout << std::endl;
// 	}
// }

// TEST_CASE("2.7 Extractor") {
// 	SECTION("equals") {
// 		using graph = gdwg::graph<int, int>;
// 		auto const v = std::vector<graph::value_type>{
// 		   {4, 1, -4},
// 		   {3, 2, 2},
// 		   {2, 4, 2},
// 		   {2, 1, 1},
// 		   {6, 2, 5},
// 		   {6, 3, 10},
// 		   {1, 5, -1},
// 		   {3, 6, -8},
// 		   {4, 5, 3},
// 		   {5, 2, 7},
// 		};
// 		auto g1 = graph(v.begin(), v.end());
// 		auto g2 = graph(v.begin(), v.end());
// 		CHECK(g1 == g2);
// 		g1.insert_node(64);
// 		CHECK(!(g1 == g2));
// 		auto g3 = graph();
// 		auto g4 = graph();
// 		CHECK(g3 == g4);
// 		auto it = g1.begin();
// 		auto ite = g1.end();
// 		while (it != ite) {
// 			std::cout << std::get<0>(*it) << " >> " << std::get<2>(*it) << " >> " <<
// std::get<1>(*it)
// 			          << std::endl;

// 			++it;
// 		}
// 		auto it1 = g1.begin();
// 		while (it1 != ite) {
// 			std::cout << std::get<0>(*it1) << " >> " << std::get<2>(*it1) << " >> "
// 			          << std::get<1>(*it1) << std::endl;

// 			it1++;
// 		}
// 		auto it2 = g1.find(3, 2, 2);
// 		while (it2 != ite) {
// 			std::cout << std::get<0>(*it2) << " >> " << std::get<2>(*it2) << " >> "
// 			          << std::get<1>(*it2) << std::endl;

// 			it2++;
// 		}
// 		std::cout << g1 << std::endl;
// 	}
// }
