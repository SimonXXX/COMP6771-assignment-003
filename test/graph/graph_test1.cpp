#include "gdwg/graph.hpp"

#include <catch2/catch.hpp>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <initializer_list>
#include <sstream>

// ================================
// CONSTRUCTORS (spec: section 2.2)
// --------------------------------

TEST_CASE("constructor 1 () - no parametersConstructors") {
	SECTION("Construct and check initialisation") {
		auto g1 = gdwg::graph<double, std::string>();
		CHECK(g1.begin() == g1.end());
		CHECK(g1.nodes().empty());
	}
}
TEST_CASE("Constructor 2 (ititializer_list)") {
	SECTION("construct and check for correct initialisation") {
		std::initializer_list<int> il1{3, 5, 7, 9, 10, 45, 3, 9};
		auto g1 = gdwg::graph<int, std::string>(il1);
		// all nodes are in the graph but no duplicates and no edges
		CHECK(ranges::all_of(il1, [&](int node) { return g1.is_node(node); }));
		CHECK(ranges::all_of(il1, [&](int node) { return g1.connections(node).empty(); }));
		CHECK(g1.nodes().size() == 6);
	}
	SECTION("check correct construction for empty list") {
		std::initializer_list<int> il2{};
		auto g2 = gdwg::graph<int, std::string>(il2);
		CHECK(g2.nodes().empty());
	}
}
TEST_CASE("Constructor 3 (range[first,last] - nodes)") {
	SECTION("construct and check for correct initialisation)") {
		using graph = gdwg::graph<char, double>;
		auto const v =
		   std::vector<char>{'4', '1', '3', '2', '2', '4', '2', '1', '6', '2', 'a', 'b', 'c'};
		auto g1 = graph(v.begin(), v.end());
		CHECK(ranges::all_of(v, [&](char node) { return g1.is_node(node); }));
		CHECK(ranges::all_of(v, [&](char node) { return g1.connections(node).empty(); }));
		CHECK(g1.nodes().size() == 8);
	}
	SECTION("check correct construction for empty list") {
		auto const v = std::vector<char>{};
		auto g1 = gdwg::graph<int, std::string>(v.begin(), v.end());
		CHECK(g1.nodes().empty());
	}
}
TEST_CASE("constructor 4 (range[first,last] - nodes and edges)") {
	SECTION("construct and check for correct initialisation)") {
		using graph = gdwg::graph<char, std::string>;
		auto const v = std::vector<graph::value_type>{{'4', '1', "-4.44"},
		                                              {'3', '2', "2.22"},
		                                              {'2', '4', "2.22"},
		                                              {'2', '1', "1.11"},
		                                              {'6', '2', "5.55"}};
		auto g1 = graph(v.begin(), v.end());
		CHECK(ranges::all_of(v, [&](graph::value_type const& vt) { return g1.is_node(vt.from); }));
		CHECK(ranges::all_of(v, [&](graph::value_type const& vt) {
			return !g1.connections(vt.from).empty();
		}));
		CHECK(g1.nodes().size() == 5);
	}
	SECTION("check with only weights with src and dst the same") {
		using graph = gdwg::graph<char, std::string>;
		auto const v = std::vector<graph::value_type>{{'4', '4', "-4.44"},
		                                              {'3', '3', "2.22"},
		                                              {'2', '2', "2.22"},
		                                              {'a', 'a', "1.11"},
		                                              {'5', '5', "5.55"}};
		auto g1 = graph(v.begin(), v.end());
		CHECK(ranges::all_of(v, [&](graph::value_type const& vt) { return g1.is_node(vt.from); }));
		CHECK(ranges::all_of(v, [&](graph::value_type const& vt) {
			return ((g1.connections(vt.from).size() == 1) and (g1.connections(vt.to).size() == 1));
		}));
		CHECK(g1.nodes().size() == 5);
	}
	SECTION("check correct construction for empty list") {
		using graph = gdwg::graph<std::string, std::string>;
		auto const v = std::vector<graph::value_type>{};
		auto g1 = graph(v.begin(), v.end());
		CHECK(g1.nodes().empty());
	}
}

TEST_CASE("Move constructor") {
	SECTION("construct graph and use it to move construct another") {
		using graph = gdwg::graph<int, double>;
		auto const v = std::vector<graph::value_type>{{4, 1, -4.44},
		                                              {3, 2, 2.22},
		                                              {2, 4, 2.22},
		                                              {2, 1, 1.11},
		                                              {6, 2, 5.55}};
		auto g1 = graph(v.begin(), v.end());
		auto g2 = std::move(g1);
		// NOLINTNEXTLINE (clang doesnt allow use of moved objects)
		CHECK(g1.empty());
		CHECK(ranges::all_of(v, [&](graph::value_type const& vt) { return g2.is_node(vt.from); }));
		g1.insert_node(42); // check this only goes into g1
		CHECK(g1.is_node(42));
		CHECK(!g2.is_node(42));
	}
	SECTION("check correct construction for empty list") {
		using graph = gdwg::graph<std::string, std::string>;
		auto const v = std::vector<std::string>{};
		auto g1 = graph(v.begin(), v.end());
		auto g2 = std::move(g1);
		// NOLINTNEXTLINE (clang doesnt allow use of moved objects)
		CHECK(g1.nodes().empty());
		CHECK(g2.nodes().empty());
	}
}
TEST_CASE("Move assignment") {
	SECTION("construct graph and move it to an empty graph") {
		using graph = gdwg::graph<double, std::string>;
		auto const v = std::vector<graph::value_type>{{4.1, 1.1, "-4.44"},
		                                              {3.1, 2.1, "2.22"},
		                                              {2.1, 4.1, "2.22"},
		                                              {2.1, 1.1, "1.11"},
		                                              {6.1, 2.1, "5.55"}};
		auto g1 = graph(v.begin(), v.end());
		auto g2 = graph();
		REQUIRE(g2.empty());
		g2 = std::move(g1);
		// NOLINTNEXTLINE (clang doesnt allow use of moved objects)
		CHECK(g1.empty());
		CHECK(ranges::all_of(v, [&](graph::value_type const& vt) { return g2.is_node(vt.from); }));
		g1.insert_node(99.9); // check this only goes into g1
		CHECK(g1.is_node(99.9));
		CHECK(!g2.is_node(99.9));
	}
	SECTION("construct graph and move it to an non-empty graph") {
		using graph = gdwg::graph<double, std::string>;
		auto const v = std::vector<graph::value_type>{{4.1, 1.1, "-4.44"},
		                                              {3.1, 2.1, "2.22"},
		                                              {2.1, 4.1, "2.22"},
		                                              {2.1, 1.1, "1.11"},
		                                              {6.1, 2.1, "5.55"}};
		auto const v1 = std::vector<double>{1.2, 3.3, 4.4, 5.5, 12.12};
		auto g1 = graph(v.begin(), v.end());
		auto g2 = graph(v1.begin(), v1.end());
		g2 = std::move(g1);
		// NOLINTNEXTLINE (clang doesnt allow use of moved objects)
		CHECK(g1.empty());
		CHECK(ranges::all_of(v, [&](graph::value_type const& vt) { return g2.is_node(vt.from); }));
		g1.insert_node(99.9); // check this only goes into g1
		CHECK(g1.is_node(99.9));
		CHECK(!g2.is_node(99.9));
	}
}

TEST_CASE("Copy constructor") {
	SECTION("construct graph and use it to copy construct another") {
		using graph = gdwg::graph<int, double>;
		auto const v = std::vector<graph::value_type>{{4, 1, -4.44},
		                                              {3, 2, 2.22},
		                                              {2, 4, 2.22},
		                                              {2, 1, 1.11},
		                                              {6, 2, 5.55}};
		auto g1 = graph(v.begin(), v.end());
		auto g2(g1);
		REQUIRE(!g1.empty());
		REQUIRE(!g2.empty());
		CHECK(ranges::all_of(v, [&](graph::value_type const& vt) { return g1.is_node(vt.from); }));
		CHECK(ranges::all_of(v, [&](graph::value_type const& vt) { return g2.is_node(vt.from); }));
		g1.insert_node(42); // check this only goes into g1
		CHECK(g1.is_node(42));
		CHECK(!g2.is_node(42));
	}
	SECTION("check correct construction for empty list") {
		using graph = gdwg::graph<std::string, std::string>;
		auto const v = std::vector<std::string>{};
		auto g1 = graph(v.begin(), v.end());
		auto const& g2(g1);
		CHECK(g1.nodes().empty());
		CHECK(g2.nodes().empty());
	}
}
TEST_CASE("Copy assignment") {
	SECTION("construct graph and copy it to an empty graph") {
		using graph = gdwg::graph<double, std::string>;
		auto const v = std::vector<graph::value_type>{{4.1, 1.1, "-4.44"},
		                                              {3.1, 2.1, "2.22"},
		                                              {2.1, 4.1, "2.22"},
		                                              {2.1, 1.1, "1.11"},
		                                              {6.1, 2.1, "5.55"}};
		auto g1 = graph(v.begin(), v.end());
		auto g2 = graph();
		REQUIRE(g2.empty());
		g2 = g1; // check that both are non-emply after copy
		CHECK(!g1.empty());
		CHECK(!g2.empty());
		CHECK(g1 == g2);
		g1.insert_node(99.9); // check this only goes into g1
		CHECK(g1.is_node(99.9));
		CHECK(!g2.is_node(99.9));
	}
	SECTION("construct graph and copy an empty graph to it") {
		using graph = gdwg::graph<double, std::string>;
		auto const v = std::vector<graph::value_type>{{4.1, 1.1, "-4.44"},
		                                              {3.1, 2.1, "2.22"},
		                                              {2.1, 4.1, "2.22"},
		                                              {2.1, 1.1, "1.11"},
		                                              {6.1, 2.1, "5.55"}};
		auto g1 = graph(v.begin(), v.end());
		auto g2 = graph();
		REQUIRE(g2.empty());
		g1 = g2; // check that both are empty after copy
		CHECK(g1.empty());
		CHECK(g2.empty());
		g1.insert_node(99.9); // check this only goes into g1
		CHECK(g1.is_node(99.9));
		CHECK(!g2.is_node(99.9));
	}

	SECTION("construct graph and copy it to an non-empty graph") {
		using graph = gdwg::graph<double, std::string>;
		auto const v = std::vector<graph::value_type>{{4.1, 1.1, "-4.44"},
		                                              {3.1, 2.1, "2.22"},
		                                              {2.1, 4.1, "2.22"},
		                                              {2.1, 1.1, "1.11"},
		                                              {6.1, 2.1, "5.55"}};
		auto const v1 = std::vector<double>{1.2, 3.3, 4.4, 5.5, 12.12};
		auto g1 = graph(v.begin(), v.end());
		auto g2 = graph(v1.begin(), v1.end());
		g2 = g1;
		REQUIRE(!g1.empty());
		CHECK(ranges::all_of(v, [&](graph::value_type const& vt) { return g2.is_node(vt.from); }));
		CHECK(g1 == g2);
		g1.insert_node(99.9); // check this only goes into g1
		CHECK(g1.is_node(99.9));
		CHECK(!g2.is_node(99.9));
	}
}
