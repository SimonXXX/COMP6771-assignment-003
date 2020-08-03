#include "gdwg/graph.hpp"
#include <catch2/catch.hpp>
#include <concepts/concepts.hpp>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <sstream>
#include <type_traits>

// =============================
// DIFFERENT types for N and E
// -----------------------------

template<concepts::regular N, concepts::regular E>
requires concepts::totally_ordered<N> //
   and concepts::totally_ordered<E> //
   auto test_different_types(std::vector<typename gdwg::graph<N, E>::value_type> const& v,
                             std::vector<N> const& nodes) {
	REQUIRE(v.size() > 2);
	using graph = gdwg::graph<N, E>;

	// construct two graphs and check they are equal
	auto g1 = graph(v.begin(), v.end());
	auto g2 = graph(v.begin(), v.end());
	CHECK(g1 == g2);
	// insert a node and check they aren't equal
	g1.insert_node(nodes[0]);
	CHECK(!(g1 == g2));
	// move graph to new graph and check old one is empty
	auto g3 = std::move(g1);
	// NOLINTNEXTLINE
	CHECK(g1.empty());
	// check iterators are set correctly
	auto it_start = g3.begin();
	auto it_finish = g3.end();
	CHECK(it_start != it_finish);
	// check that iterators can find corret edges
	auto it = g2.begin();
	CHECK(static_cast<N>(std::get<0>(*it)) == v[0].from);
	CHECK(static_cast<N>(std::get<1>(*it)) == v[0].to);
	CHECK(static_cast<E>(std::get<2>(*it)) == v[0].weight);
	it++;
	it++;
	CHECK(static_cast<N>(std::get<0>(*it)) == v[2].from);
	CHECK(static_cast<N>(std::get<1>(*it)) == v[2].to);
	CHECK(static_cast<E>(std::get<2>(*it)) == v[2].weight);
}

TEST_CASE("Test different type combinations") {
	SECTION("int int") {
		auto const v = std::vector<gdwg::graph<int, int>::value_type>{{2, 4, 2}, {3, 2, 2}, {4, 1, -4}};
		auto const nodes = std::vector<int>{64, 77, 42};
		test_different_types<int, int>(v, nodes);
	}
	SECTION("int, std::string") {
		auto const v = std::vector<gdwg::graph<int, std::string>::value_type>{{1, 2, "3"},
		                                                                      {4, 5, "6"},
		                                                                      {7, 8, "9"}};
		auto const nodes = std::vector<int>{64, 77, 42};
		test_different_types<int, std::string>(v, nodes);
	}

	SECTION("std::string, std::string") {
		auto const v = std::vector<gdwg::graph<std::string, std::string>::value_type>{{"1", "2", "3"},
		                                                                              {"4", "5", "6"},
		                                                                              {"7", "8", "9"}};
		auto const nodes = std::vector<std::string>{"1066", "77", "42"};
		test_different_types<std::string, std::string>(v, nodes);
	}
	SECTION("char, double") {
		auto const v = std::vector<gdwg::graph<char, double>::value_type>{{'a', 'b', 2.123},
		                                                                  {'c', 'd', 42.42},
		                                                                  {'e', 'f', 999.42}};
		auto const nodes = std::vector<char>{'x', 'y', 'z'};
		test_different_types<char, double>(v, nodes);
	}
	// SECTION("std::vector<int>, double") {
	// 	auto const v = std::vector<gdwg::graph<std::vector<int>, double>::value_type>{
	// 	   {std::vector<int>{2, 2, 3}, std::vector<int>{1, 2, 3}, 2.123},
	// 	   {std::vector<int>{3, 2, 3}, std::vector<int>{2, 2, 3}, 42.42},
	// 	   {std::vector<int>{4, 2, 3}, std::vector<int>{3, 2, 3}, 999.42}};
	// 	auto const nodes = std::vector<std::vector<int>>{std::vector<int>{5, 2, 3},
	// 	                                                 std::vector<int>{6, 2, 3},
	// 	                                                 std::vector<int>{7, 2, 3}};
	// 	test_different_types<std::vector<int>, double>(v, nodes);
	// }
}