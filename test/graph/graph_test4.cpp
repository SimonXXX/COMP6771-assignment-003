#include "gdwg/graph.hpp"
#include <catch2/catch.hpp>
#include <concepts/concepts.hpp>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <sstream>
#include <type_traits>

// =============================
// ITERATOR (spec: section 2.8)
// -----------------------------

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


