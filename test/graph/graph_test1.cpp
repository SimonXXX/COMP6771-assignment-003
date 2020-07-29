#include "gdwg/graph.hpp"

#include <catch2/catch.hpp>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <sstream>

TEST_CASE("basic test") {
	auto g1 = gdwg::graph<int, std::string>{};
	auto n = 5;
	g1.insert_node(n);
	CHECK(g1.is_node(n));
	g1.insert_edge(5, 5, "weight1");
	// edge "weight1" pointing to/from node 3 which has 3 ptrs
	// CHECK(g1.test_edge_details("weight1") == "5(3) | weight1 | 5(3) ");

	g1.insert_node(10);
	g1.insert_edge(10, 5, "weight2");
	// CHECK(g1.test_edge_details("weight2") == "10(2) | weight2 | 5(4) ");

	std::cout << g1;

	g1.replace_node(10, 12);
	std::cout << g1;

	auto g = gdwg::graph<std::string, int>{};
	g.insert_node("hello");
	g.insert_node("how");
	g.insert_node("are");
	g.insert_node("you?");

	g.insert_edge("hello", "how", 5);
	g.insert_edge("hello", "are", 8);
	g.insert_edge("hello", "are", 2);
	g.insert_edge("hello", "are", 4);
	g.insert_edge("are", "are", 8);
	// std::cout << g.test_edge_details(8) << std::endl;
	// std::cout << g.test_edge_details(8) << std::endl;
}

TEST_CASE("2.7 Extractor") {
	using graph = gdwg::graph<int, int>;
	auto const v = std::vector<graph::value_type>{
	   {4, 1, -4},
	   {3, 2, 2},
	   {2, 4, 2},
	   {2, 1, 1},
	   {6, 2, 5},
	   {6, 3, 10},
	   {1, 5, -1},
	   {3, 6, -8},
	   {4, 5, 3},
	   {5, 2, 7},
	};

	auto g = graph(v.begin(), v.end());
	g.insert_node(64);
	auto out = std::ostringstream{};
	out << g;
	auto const expected_output = std::string_view(R"(1 (
  5 | -1
)
2 (
  1 | 1
  4 | 2
)
3 (
  2 | 2
  6 | -8
)
4 (
  1 | -4
  5 | 3
)
5 (
  2 | 7
)
6 (
  2 | 5
  3 | 10
)
64 (
)
)");
	CHECK(out.str() == expected_output);
	// g.print_edges();
}

TEST_CASE("2.3 Modifiers [gdwg.modifiers]") {
	SECTION("auto replace_node(N const& old_data, N const& new_data) -> bool") {
		// Effects: Replaces the original data with replacement data.
		// Does nothing if new_data already exists as a node.
		// Postconditions: All iterators are invalidated.
		// Returns: false if a node that contains value new_data already exists and true otherwise.
		// Throws: std::runtime_error if the original data doesnt exist on any node
		auto g1 = gdwg::graph<int, std::string>{};
		g1.insert_node(5);
		g1.insert_edge(5, 5, "edge 1");
		g1.insert_node(10);
		g1.insert_edge(10, 5, "edge 2");
		REQUIRE(g1.is_node(5));
		REQUIRE(g1.is_node(10));
		CHECK(g1.replace_node(10, 12));
		CHECK(g1.is_node(5));
		CHECK(g1.is_node(12));
		CHECK(!g1.is_node(10));
		CHECK(!g1.replace_node(10, 12));
		// CHECK_THROWS_WITH(g1.replace_node(15, 16),
		//                 "Cannot call gdwg::graph<N, E>::replace_node on a node that doesn't
		//                 exist");
	}
	SECTION("auto merge_replace_node(N const& old_data, N const& new_data) -> void") {
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
		std::cout << g1 << std::endl;
		g1.merge_replace_node('A', 'B');
		std::cout << g1 << std::endl;
		g1.print_edges_test();

		// Effects: The node equivalent to old_data in the graph are replaced with instances of
		// new_data. After completing, every incoming and outgoing edge of old_data becomes an
		// incoming/ougoing edge of new_data, except that duplicate edges shall be removed.
		// Postconditions: All iterators are invalidated.
		// Throws: std::runtime_error("Cannot call gdwg::graph<N, E>::merge_replace_node on old or new
		// data if they don't exist in the graph") if either of is_node(old_data) or is_node(new_data)
		// are false. [Note: Unlike Assignment 2, the exception message must be used verbatim. ??end
		// note] [Note: The following examples use the format (Nsrc,??Ndst,??E). [Example: Basic
		// example.
	}
	SECTION("erase_node") {
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
		std::cout << g1 << std::endl;
		CHECK(g1.erase_node('A'));
		std::cout << g1 << std::endl;
		g1.print_edges_test();
		CHECK(not g1.erase_node('A'));
	}
	SECTION("erase_edge") {
		auto g1 = gdwg::graph<char, int>{};
		g1.insert_node('A');
		g1.insert_node('B');
		g1.insert_node('C');
		g1.insert_node('D');
		g1.insert_edge('A', 'B', 1);
		g1.insert_edge('A', 'C', 2);
		g1.insert_edge('A', 'D', 3);
		g1.insert_edge('D', 'C', 22);
		g1.print_edges_test();
		g1.erase_edge('D', 'C', 22);
		g1.print_edges_test();
		g1.clear();
		g1.print_edges_test();
	}
	SECTION("is_connected") {
		auto v = std::vector<char>{'A', 'B', 'C', 'D', 'a', 'b', 'c'};
		auto g1 = gdwg::graph<char, int>(v.begin(), v.end());
		g1.insert_edge('A', 'B', 1);
		g1.insert_edge('A', 'C', 2);
		g1.insert_edge('A', 'D', 3);
		g1.insert_edge('D', 'C', 22);
		CHECK(!g1.is_connected('a', 'b')); // neither nodes connected to anything
		g1.insert_edge('a', 'C', 22);
		CHECK(!g1.is_connected('a', 'b')); // first node connected, second no edges
		CHECK(!g1.is_connected('c', 'a')); // second node connected, first no edges
		g1.insert_edge('b', 'C', 22);
		CHECK(!g1.is_connected('a', 'b')); // both nodes connected but not to each other
		CHECK(g1.is_connected('D', 'C')); // nodes connected to each other
		CHECK_THROWS_WITH(g1.is_connected('A', 'E'),
		                  "Cannot call gdwg::graph<N, E>::is_connected if src or dst node don't "
		                  "exist in the graph");
		CHECK_THROWS_WITH(g1.is_connected('E', 'A'),
		                  "Cannot call gdwg::graph<N, E>::is_connected if src or dst node don't "
		                  "exist in the graph");
		for (auto i : g1.nodes()) {
			std::cout << i << " ";
		}
		g1.insert_edge('A', 'C', 12);
		g1.insert_edge('A', 'C', 22);
		g1.insert_edge('A', 'C', 32);
		g1.insert_edge('A', 'C', 42);
		g1.insert_edge('A', 'C', -12121212);
		std::cout << std::endl;

		for (auto i : g1.weights('A', 'C')) {
			std::cout << i << " ";
		}
		std::cout << std::endl;
	}
}