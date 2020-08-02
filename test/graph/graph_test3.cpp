#include "gdwg/graph.hpp"
#include <catch2/catch.hpp>
#include <concepts/concepts.hpp>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <sstream>
#include <type_traits>

TEST_CASE("iterator test") {
	SECTION("") {
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
		auto g1 = graph(v.begin(), v.end());
		auto const& g2 = std::as_const(g1);
		{
			// Test for equality
			REQUIRE(g1.begin() != g1.end());

			REQUIRE(g2.begin() != g2.end());
			REQUIRE(g1.begin() == g2.begin());
			REQUIRE(g2.begin() == g1.begin());
			REQUIRE(g1.end() == g2.end());
			REQUIRE(g2.end() == g1.end());
		}
	}
}
TEST_CASE("edge erase") {
	SECTION("") {
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
		auto g1 = graph(v.begin(), v.end());
		auto i = g1.find(2, 4, 2);
		auto s = g1.find(4, 1, -4);
		std::cout << std::endl;
		auto x = g1.erase_edge(i, s);
		std::cout << std::get<0>(*x) << " >> " << std::get<2>(*x) << " >> " << std::get<1>(*x)
		          << std::endl;

		for (auto const& [from, to, weight] : g1) {
			std::cout << from << std::endl;
		}
	}
	SECTION("") {
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
		auto g1 = graph(v.begin(), v.end());
		auto it = g1.find(2, 1, 1);
		CHECK(std::get<0>(*it) == 2);
		++it;
		CHECK(std::get<0>(*it) == 2);
		CHECK(std::get<1>(*it) == 4);
		auto it1 = g1.end();
		CHECK(std::get<0>(*--it1) == 6);
	}
}

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
