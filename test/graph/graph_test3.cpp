#include "gdwg/graph.hpp"

#include <catch2/catch.hpp>
#include <concepts/concepts.hpp>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <linux/limits.h>
#include <range/v3/algorithm/is_sorted.hpp>
#include <sstream>

// =============================
// Accessors section 2.4)
// -----------------------------

TEST_CASE("Accessor 1 (checks if a value represents a node)") {
	SECTION("Basic check as this has been extensively tested in other test_cases") {
		using graph = gdwg::graph<double, char>;
		auto v = std::vector<graph::value_type>{{12.12, 99.9, '4'}, {1.1, 2.2, '3'}, {7.7, 7.7, 'a'}};
		auto g1 = graph(v.begin(), v.end());
		auto dbl_var = 999.999;
		CHECK(!g1.is_node(dbl_var));
		REQUIRE(g1.insert_node(dbl_var));
		CHECK(g1.is_node(dbl_var));
		CHECK(ranges::all_of(v, [&](graph::value_type node) { return g1.is_node(node.from); }));
		CHECK(ranges::all_of(v, [&](graph::value_type node) { return g1.is_node(node.to); }));
	}
	SECTION("test on empty graph") {
		using graph1 = gdwg::graph<int, std::string>;
		auto g1 = graph1();
		REQUIRE(g1.empty());
		CHECK(!g1.is_node('a'));
		CHECK(!g1.is_node(1));
		CHECK(!g1.is_node('\u0000'));
		using graph2 = gdwg::graph<std::string, double>;
		auto g2 = graph2();
		REQUIRE(g2.empty());
		CHECK(!g2.is_node(""));
		CHECK(!g2.is_node("Hello"));
	}
}

TEST_CASE("Accessor 2 (checks if the graph is empty") {
	SECTION("Basic checks as this has been extensively tested in other test_cases") {
		using graph = gdwg::graph<int, std::string>;
		auto g1 = graph();
		CHECK(g1.empty());
		REQUIRE(g1.insert_node(42));
		REQUIRE(g1.insert_node(999));
		REQUIRE(g1.insert_edge(42, 999, "Hello"));
		CHECK(!g1.empty());
		std::initializer_list<int> il1{};
		auto g2 = graph(il1);
		CHECK(g2.empty());
		REQUIRE(g2.insert_node(42));
		CHECK(!g2.empty());
	}
}

TEST_CASE("Accessor 3 (checks if two nodes are connected") {
	SECTION("Check basic connections") {
		using graph = gdwg::graph<int, char>;
		auto v = std::vector<graph::value_type>{{12, 99, '4'}, {1, 2, '3'}, {7, 7, 'a'}};
		auto g1 = graph(v.begin(), v.end());
		CHECK(g1.is_connected(12, 99));
		CHECK(g1.is_connected(1, 2));
		CHECK(g1.is_connected(7, 7));
	}
	SECTION("Check non existant nodes and edges") {
		using graph = gdwg::graph<int, char>;
		auto v = std::vector<graph::value_type>{{12, 99, '4'}, {1, 2, '3'}, {7, 7, 'a'}};
		auto g1 = graph(v.begin(), v.end());
		REQUIRE(!g1.is_connected(99, 12));
		REQUIRE(!g1.is_connected(2, 1));
		CHECK_THROWS_WITH(g1.is_connected(3, 4),
		                  "Cannot call gdwg::graph<N, E>::is_connected if src or dst node don't "
		                  "exist in the graph");
		CHECK_THROWS_WITH(g1.is_connected(12, 4),
		                  "Cannot call gdwg::graph<N, E>::is_connected if src or dst node don't "
		                  "exist in the graph");
		CHECK_THROWS_WITH(g1.is_connected(3, 1),
		                  "Cannot call gdwg::graph<N, E>::is_connected if src or dst node don't "
		                  "exist in the graph");
		CHECK(!g1.is_connected(7, 99));
		CHECK(!g1.is_connected(7, 12));
		CHECK(!g1.is_connected(7, 1));
		CHECK(!g1.is_connected(7, 2));
	}
}

TEST_CASE("Accessor 4 (returns a sequence of nodes") {
	SECTION("Check with populated graph") {
		using graph = gdwg::graph<char, double>;
		std::initializer_list<char> il1{'a', 'j', 'c', 'g', 'g', 'b', 'i', 'f', 'd', 'e'};
		auto g1 = graph(il1);
		auto v = g1.nodes();
		REQUIRE(!v.empty());
		CHECK(ranges::is_sorted(v));
	}
	SECTION("Check with empty graph") {
		using graph = gdwg::graph<char, double>;
		auto g1 = graph();
		auto v = g1.nodes();
		REQUIRE(v.empty());
	}
}

TEST_CASE("Accessor 5 (returns a sequence of weights)") {
	SECTION("Check with populated graph") {
		using graph = gdwg::graph<int, int>;
		auto const v = std::vector<graph::value_type>{
		   {1, 5, -1},
		   {2, 1, 1},
		   {2, 4, 2},
		   {3, 2, 2},
		   {3, 5, 2},
		   {3, 6, -8},
		   {4, 1, -4},
		   {4, 5, 3},
		   {5, 2, 7},
		   {6, 2, 5},
		   {6, 3, 10},
		};
		auto g1 = graph(v.begin(), v.end());
		auto wts = g1.weights(3, 5); // only 1 edge
		CHECK(!wts.empty());
		CHECK(wts[0] == 2);
		wts = g1.weights(5, 1); // no edge
		CHECK(wts.empty());
		REQUIRE(g1.insert_edge(4, 5, 42));
		REQUIRE(g1.insert_edge(4, 5, 6));
		REQUIRE(g1.insert_edge(4, 5, 1));
		REQUIRE(g1.insert_edge(4, 5, 999));
		wts = g1.weights(4, 5); // multiple edges
		CHECK(wts.size() == 5);
		CHECK(ranges::is_sorted(wts));
	}
	SECTION("Check with non existant nodes") {
		using graph = gdwg::graph<char, double>;
		auto g1 = graph();
		auto v = g1.nodes();
		CHECK_THROWS_WITH(g1.weights('5', '1'),
		                  "Cannot call gdwg::graph<N, E>::weights if src or dst node don't exist in "
		                  "the graph");
		REQUIRE(g1.insert_node('5'));
		CHECK_THROWS_WITH(g1.weights('5', '1'),
		                  "Cannot call gdwg::graph<N, E>::weights if src or dst node don't exist in "
		                  "the graph");
		CHECK_THROWS_WITH(g1.weights('1', '5'),
		                  "Cannot call gdwg::graph<N, E>::weights if src or dst node don't exist in "
		                  "the graph");
	}
}
TEST_CASE("Accessor 6 (return an iterator to an edge)") {
	SECTION("Check finds correct edge") {
		using graph = gdwg::graph<int, char>;
		auto const v = std::vector<graph::value_type>{
		   {1, 5, '-'},
		   {2, 1, '1'},
		   {2, 4, '%'},
		   {5, 2, '7'},
		   {6, 2, '5'},
		   {6, 3, '@'},
		};
		auto g1 = graph(v.begin(), v.end());
		auto it = g1.find(2, 4, '%'); // edge exists
		CHECK(std::get<0>(*it) == 2);
		CHECK(std::get<1>(*it) == 4);
		CHECK(std::get<2>(*it) == '%');
	}
	SECTION("Check when no edge can be found") {
		using graph = gdwg::graph<int, char>;
		auto const v = std::vector<graph::value_type>{
		   {1, 5, '-'},
		   {2, 1, '1'},
		   {2, 4, '%'},
		   {5, 2, '7'},
		   {6, 2, '5'},
		   {6, 3, '@'},
		};
		auto g1 = graph(v.begin(), v.end());
		auto it1 = g1.find(2, 4, '$'); // edge doesnt exists - nodes connected
		CHECK(it1 == g1.end());
		auto it2 = g1.find(6, 4, '4'); // edge doesnt exists - nodes not connected
		CHECK(it2 == g1.end());
		auto it3 = g1.find(7, 4, '4'); // node doesnt exist
		CHECK(it3 == g1.end());
		auto it4 = g1.find(4, 7, '4'); // node doesnt exist
		CHECK(it4 == g1.end());
		auto it5 = g1.find(7, 8, '4'); // both nodes dont exist
		CHECK(it5 == g1.end());
	}
}
TEST_CASE("Accessor 7 (returns a sequence of nodes connected to a given node)") {
	SECTION("Check finds correct nodes") {
		using graph = gdwg::graph<int, int>;
		auto const v = std::vector<graph::value_type>{
		   {1, 5, -1},
		   {2, 1, 1},
		   {2, 4, 2},
		   {3, 2, 2},
		   {3, 5, 2},
		   {3, 6, -8},
		   {4, 1, -4},
		   {4, 5, 3},
		   {5, 2, 7},
		   {6, 2, 5},
		   {6, 3, 10},
		};
		auto g1 = graph(v.begin(), v.end());
		auto cons = g1.connections(3);
		CHECK(cons.size() == 3);
		CHECK(ranges::is_sorted(cons));
		cons = g1.connections(1);
		CHECK(cons.size() == 1);
		REQUIRE(g1.insert_node(42));
		cons = g1.connections(42);
		CHECK(cons.empty());
	}
	SECTION("Check with non-existant nodes") {
		using graph = gdwg::graph<char, double>;
		auto g1 = graph();
		auto v = g1.nodes();
		CHECK_THROWS_WITH(g1.connections(1),
		                  "Cannot call gdwg::graph<N, E>::connections if src doesn't exist in the "
		                  "graph");
		REQUIRE(g1.insert_node('5'));
		REQUIRE(g1.insert_node('4'));
		auto cons = g1.connections('5'); // two nodes and no edges
		CHECK(cons.empty());
		CHECK_THROWS_WITH(g1.connections('3'),
		                  "Cannot call gdwg::graph<N, E>::connections if src doesn't exist in the "
		                  "graph");
	}
}
// ==========================
// RANGE ACCESS (section 2.5)
// --------------------------

TEST_CASE("Range access (begin() and end())- extensively tested as part of other tests") {
	SECTION("Check with populated graph") {
		using graph = gdwg::graph<int, char>;
		auto const v = std::vector<graph::value_type>{
		   {1, 5, '-'},
		   {2, 1, '1'},
		   {2, 4, '%'},
		   {5, 2, '7'},
		   {6, 2, '5'},
		   {6, 3, '@'},
		};
		auto g1 = graph(v.begin(), v.end());
		auto it = g1.begin();
		CHECK(std::get<0>(*it) == 1);
		CHECK(std::get<1>(*it) == 5);
		CHECK(std::get<2>(*it) == '-');
		auto it1 = g1.end();
		--it1;
		CHECK(std::get<0>(*it1) == 6);
		CHECK(std::get<1>(*it1) == 3);
		CHECK(std::get<2>(*it1) == '@');
	}

	SECTION("Check with empty graph and one edge graph") {
		using graph = gdwg::graph<char, double>;
		auto g1 = graph();
		auto v = g1.nodes();
		CHECK(g1.begin() == g1.end());
		REQUIRE(g1.insert_node('5'));
		CHECK(g1.begin() == g1.end());
		REQUIRE(g1.insert_node('4'));
		CHECK(g1.begin() == g1.end());
		REQUIRE(g1.insert_edge('5', '4', 42.42));
		CHECK(g1.begin() != g1.end());
		auto it = g1.begin();
		REQUIRE(it == --g1.end());
		CHECK(std::get<0>(*it) == '5');
		CHECK(std::get<1>(*it) == '4');
		CHECK(std::get<2>(*it) == 42.42);
	}
}
