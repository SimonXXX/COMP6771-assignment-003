#include "gdwg/graph.hpp"

#include <catch2/catch.hpp>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <sstream>

TEST_CASE("2.7 Extractor") {
	SECTION("equals") {
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
		auto g2 = graph(v.begin(), v.end());
		CHECK(g1 == g2);
		g1.insert_node(64);
		CHECK(!(g1 == g2));
		auto g3 = graph();
		auto g4 = graph();
		CHECK(g3 == g4);
		auto it = g1.begin();
		auto ite = g1.end();
		while (it != ite) {
			std::cout << std::get<0>(*it) << " >> " << std::get<2>(*it) << " >> " << std::get<1>(*it)
			          << std::endl;

			++it;
		}
		auto it1 = g1.begin();
		while (it1 != ite) {
			std::cout << std::get<0>(*it1) << " >> " << std::get<2>(*it1) << " >> "
			          << std::get<1>(*it1) << std::endl;

			it1++;
		}
		auto it2 = g1.find(3, 2, 2);
		while (it2 != ite) {
			std::cout << std::get<0>(*it2) << " >> " << std::get<2>(*it2) << " >> "
			          << std::get<1>(*it2) << std::endl;

			it2++;
		}
		std::cout << g1 << std::endl;
	}
}
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