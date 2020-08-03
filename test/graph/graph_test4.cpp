#include "gdwg/graph.hpp"
#include <catch2/catch.hpp>
#include <concepts/concepts.hpp>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <sstream>
#include <type_traits>

// =========================
// COMPARISONS (section 2.6)
// -------------------------

TEST_CASE("The equality operator") {
	SECTION("Check for populated graphs with edges") {
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
		auto g2 = graph(v.begin(), v.end());
		CHECK(g1 == g2);
		REQUIRE(g1.insert_node(42));
		CHECK(g1 != g2);
		REQUIRE(g2.insert_node(42));
		CHECK(g1 == g2);
		g1.clear();
		CHECK(g1 != g2);
		g2.clear();
		CHECK(g1 == g2);
	}
	SECTION("Check for empty graphs") {
		using graph = gdwg::graph<std::string, std::string>;
		auto g1 = graph();
		auto g2 = graph();
		CHECK(g1 == g2);
		REQUIRE(g1.insert_node("42"));
		CHECK(g1 != g2);
		REQUIRE(g2.insert_node("42"));
		CHECK(g1 == g2);
	}
}

// ========================
// EXTRACTOR (section 2.7)
// ------------------------
TEST_CASE("Formatted output function - this has been tested in previous tests") {
	SECTION("Check for populated graphs with edges") {
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
		auto out = std::ostringstream{};
		out << g1;
		auto const expected_output = std::string_view(R"(1 (
  5 | -
)
2 (
  1 | 1
  4 | %
)
3 (
)
4 (
)
5 (
  2 | 7
)
6 (
  2 | 5
  3 | @
)
)");
		CHECK(out.str() == expected_output);
	}
	SECTION("Check for empty graphs") {
		using graph = gdwg::graph<std::string, std::string>;
		auto g1 = graph();
		auto g2 = graph();
		auto out = std::ostringstream{};
		out << g1;
		auto const expected_output = std::string_view(R"()");
		CHECK(out.str() == expected_output);
		REQUIRE(g1.insert_node("42"));
		out << g1;
		auto const expected_output1 = std::string_view(R"(42 (
)
)");
		CHECK(out.str() == expected_output1);
	}
}
// ======================
// ITERATOR (section 2.8)
// ----------------------

TEST_CASE("The graph iterator has been extensively been tested in other tests - basic tests only") {
	SECTION("Test for equality using graph with nodes and edges") {
		using graph = gdwg::graph<int, int>;
		auto const v = std::vector<graph::value_type>{
		   {4, 1, -4},
		   {3, 2, 2},
		   {3, 6, -8},
		   {4, 5, 3},
		   {5, 2, 7},
		};
		auto g1 = graph(v.begin(), v.end());
		auto const& g2 = g1;
		CHECK(g1.begin() != g1.end());
		CHECK(g2.begin() != g2.end());
		CHECK(g1.begin() == g2.begin());
		CHECK(g2.begin() == g1.begin());
		CHECK(g1.end() == g2.end());
		CHECK(g2.end() == g1.end());
	}
	SECTION("Test for equality using graph with nodes") {
		using graph = gdwg::graph<char, char>;
		auto g1 = graph();
		REQUIRE(g1.insert_node('a'));
		REQUIRE(g1.insert_node('A'));
		REQUIRE(g1.insert_node('@'));
		REQUIRE(g1.insert_node('#'));
		auto const& g2 = g1;
		CHECK(g1.begin() == g1.end()); // only looks at nodes with edges
		CHECK(g2.begin() == g2.end());
		CHECK(g1.begin() == g2.begin());
		CHECK(g2.begin() == g1.begin());
		CHECK(g1.end() == g2.end());
		CHECK(g2.end() == g1.end());
	}
	SECTION("Test for equality using empty graph") {
		using graph = gdwg::graph<char, char>;
		auto g1 = graph();
		auto const& g2 = g1;
		CHECK(g1.begin() == g1.end());
		CHECK(g2.begin() == g2.end());
		CHECK(g1.begin() == g2.begin());
		CHECK(g2.begin() == g1.begin());
		CHECK(g1.end() == g2.end());
		CHECK(g2.end() == g1.end());
	}

	SECTION("Check iterator source operator and ++, -- operators") {
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
		++it;
		++it;
		CHECK(std::get<0>(*it) == 2);
		CHECK(std::get<1>(*it) == 4);
		CHECK(std::get<2>(*it) == '%');
		--it;
		CHECK(std::get<0>(*it) == 2);
		CHECK(std::get<1>(*it) == 1);
		CHECK(std::get<2>(*it) == '1');
		auto it1 = g1.end();
		--it1;
		CHECK(std::get<0>(*it1) == 6);
		CHECK(std::get<1>(*it1) == 3);
		CHECK(std::get<2>(*it1) == '@');
	}
	SECTION("Check iterator source operator and ++(int), --(int) operators") {
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
		it++;
		it++;
		CHECK(std::get<0>(*it) == 2);
		CHECK(std::get<1>(*it) == 4);
		CHECK(std::get<2>(*it) == '%');
		it--;
		CHECK(std::get<0>(*it) == 2);
		CHECK(std::get<1>(*it) == 1);
		CHECK(std::get<2>(*it) == '1');
		auto it1 = g1.end();
		it1--;
		CHECK(std::get<0>(*it1) == 6);
		CHECK(std::get<1>(*it1) == 3);
		CHECK(std::get<2>(*it1) == '@');
	}
}
