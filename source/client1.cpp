#include <iostream>
#include <string>
#include <vector>

#include "gdwg/graph.hpp"
<<<<<<< HEAD

using gdwg::graph;
=======
>>>>>>> 82339a6ef6c15a7ce85b3dd289ff351b16cd1062

auto main() -> int {
	// auto v = std::vector<std::string>{"df", "er", "hhh", "jjj", "ggggg", "yrdtrt", "dfg", "ff"};
	// auto g = graph<std::string, int>{v.begin(), v.end()};
	auto g = graph<std::string, int>{};
	g.insert_node("hello");
	g.insert_node("how");
	g.insert_node("are");
	g.insert_node("you?");

	// g.print_vals();

	g.insert_edge("hello", "how", 5);
	g.insert_edge("hello", "are", 8);
	g.insert_edge("hello", "are", 2);
	g.insert_edge("hello", "are", 4);
	// g.print_vals();

	// g.insert_edge("how", "you?", 1);
	// g.insert_edge("how", "hello", 4);

	// g.insert_edge("are", "you?", 3);

	// std::cout << g << "\n";
	auto g1 = graph<int, std::string>{};
	g1.insert_node(1000);
	g1.insert_node(11000);
	g1.insert_node(111000);
	g1.insert_node(122000);
	std::cout << (g1.insert_edge(1000, 122000, "hello") ? "true" : "false") << "\n";
	std::cout << (g1.insert_edge(11000, 122000, "hello") ? "true" : "false") << "\n";
	std::cout << (g1.insert_edge(111000, 122000, "hello") ? "true" : "false") << "\n";
	std::cout << (g1.insert_edge(11000, 122000, "brilliant") ? "true" : "false") << "\n";
	g1.insert_edge(122000, 122000, "awesome");
	g1.print_vals_test();
	g1.print_edges_test();

	// auto g2 = gdwg::graph<std::string, int>{g};

<<<<<<< HEAD
	// auto g3 = gdwg::graph<std::string, int>{"hello"};
	// g3.print_vals();
=======
	auto g2 = gdwg::graph<std::string, int>(g);
>>>>>>> 82339a6ef6c15a7ce85b3dd289ff351b16cd1062

	// std::cout << g2 << "\n";

	// struct xxxx {
	// 	int iterator = 0;
	// 	int from = 1;
	// 	int to = 2;
	// 	int weight = 3;
	// };
	std::vector<int> gg = {1, 2, 3, 4};
	// This is a structured binding.
	// https://en.cppreference.com/w/cpp/language/structured_binding
	// It allows you to unpack your tuple.
	// auto [from, to] = g;
	// for (auto const& [from, to, weight] : g) {
	// 	std::cout << from << " -> " << to << " (weight " << weight << ")\n";
	// for (auto const& [one, two] : g) {
	//	std::cout << one.from << " -> " << two.to << " (weight " << two.weight << ")\n";
	//}
}
