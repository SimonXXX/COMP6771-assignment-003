#include "gdwg/graph.hpp"
#include <catch2/catch.hpp>
#include <concepts/concepts.hpp>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <sstream>
#include <type_traits>

template<concepts::regular N, concepts::regular E>
requires concepts::totally_ordered<N> //
   and concepts::totally_ordered<E> //
   auto test_different_types(std::vector<typename gdwg::graph<N, E>::value_type> const& v,
                             std::vector<N> const& nodes) {
	REQUIRE(v.size() > 2);
	using graph = gdwg::graph<N, E>;

	auto g1 = graph(v.begin(), v.end());
	auto g2 = graph(v.begin(), v.end());
	CHECK(g1 == g2);
	g1.insert_node(nodes[0]);
	CHECK(!(g1 == g2));
	auto g3 = std::move(g1);
	// NOLINTNEXTLINE
	CHECK(g1.empty());

	auto it_start = g3.begin();
	auto it_finish = g3.end();
	CHECK(it_start != it_finish);
	auto it = g3.begin();
	it++;
	it++;
	std::cout << g3.edge_details_test(v[2]) << std::endl;
	// std::cout << std::get<0>(*it) << std::endl;
	// auto i = 1;
	// while (i < 11) {
	// 	std::cout << (*it) << std::endl;
	// 	++it;
	// 	i++;
	// }
}
TEST_CASE("") {
	SECTION("int int") {
		auto const v = std::vector<gdwg::graph<int, int>::value_type>{
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
	// SECTION("") {
	// auto const v = std::vector<gdwg::graph<>::value_type>{
	//{,,,},
	// };
	// test_different_types<>(v);
	//}
}
