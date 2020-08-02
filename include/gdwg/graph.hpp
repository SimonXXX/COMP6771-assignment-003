#ifndef GDWG_GRAPH_HPP
#define GDWG_GRAPH_HPP

#include <__functional_base>
#include <__tuple>
#include <algorithm>
#include <concepts/concepts.hpp>
#include <fmt/ostream.h>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <limits>
#include <memory>
#include <ostream>
#include <pthread.h>
#include <range/v3/algorithm.hpp>
#include <range/v3/iterator.hpp>
#include <range/v3/utility.hpp>
#include <set>
#include <stdexcept>
#include <tuple>
#include <type_traits>

namespace gdwg {

	template<concepts::regular N, concepts::regular E>
	requires concepts::totally_ordered<N> //
	   and concepts::totally_ordered<E> //
	   class graph {
	public:
		// struct for edge data
		struct value_type {
			N from;
			N to;
			E weight;
			[[nodiscard]] auto operator==(value_type const& other) const -> bool {
				return static_cast<bool>((this->from == other.from) and (this->to == other.to)
				                         and (this->weight == other.weight));
			}
		};

		class iterator; // forward declaration of iterator class

		//   =============
		//   INNER CLASSES
		//   -------------
		//
		//   ----------
		//   NODE CLASS
		//   ----------
		class node {
		public:
			// node constructors
			node() {
				node_value_ = N{};
			}
			explicit node(N const& val) {
				node_value_ = val;
			}
			// node setters
			void set_node_value(N const& val) {
				node_value_ = val;
			}
			// node getters
			[[nodiscard]] auto get_node_value() const -> N {
				return node_value_;
			}

		private:
			N node_value_{};
		};

		//   ----------
		//   EDGE CLASS
		//   ----------
		class edge {
		public:
			// edge constructors
			edge() = delete;

			edge(std::shared_ptr<node> f, std::shared_ptr<node> t, E w)
			: weight_{w} {
				from_ptr_ = std::move(f);
				to_ptr_ = std::move(t);
			}

			// edge getters
			[[nodiscard]] auto get_edge_weight() const -> E {
				return weight_;
			}
			[[nodiscard]] auto get_from_node() const -> N {
				return from_ptr_->get_node_value();
			}
			[[nodiscard]] auto get_to_node() const -> N {
				return to_ptr_->get_node_value();
			}
			[[nodiscard]] auto get_from_count() const -> long {
				return from_ptr_.use_count();
			}
			[[nodiscard]] auto get_to_count() const -> long {
				return to_ptr_.use_count();
			}
			[[nodiscard]] auto get_edge_details() const -> value_type {
				return value_type{from_ptr_->get_node_value(), to_ptr_->get_node_value(), weight_};
			}

			// edge setters
			auto set_from_ptr(std::shared_ptr<node> const& new_node_ptr) {
				from_ptr_.reset();
				from_ptr_ = std::move(new_node_ptr);
			}
			auto set_to_ptr(std::shared_ptr<node> const& new_node_ptr) {
				to_ptr_.reset();
				to_ptr_ = std::move(new_node_ptr);
			}

		private:
			std::shared_ptr<node> from_ptr_;
			std::shared_ptr<node> to_ptr_;
			E weight_;
		};
		// --------------------
		// END OF INNER CLASSES
		// ====================
		//

		// ================================
		// CONSTRUCTORS (spec: section 2.2)
		// --------------------------------
		// constructor 1 ()
		graph<N, E>() = default;

		// constructor 2 (ititializer_list)
		graph(std::initializer_list<N> il)
		: graph(il.begin(), il.end()) {}

		// constructor 3 (range[first,last] - nodes)
		template<ranges::forward_iterator I, ranges::sentinel_for<I> S>
		requires ranges::indirectly_copyable<I, N*> graph<N, E>(I first, S last) {
			std::for_each(first, last, [this](N const& n) { insert_node(n); });
		}

		// constructor 4 (range[first,last] - nodes and edges)
		template<ranges::forward_iterator I, ranges::sentinel_for<I> S>
		requires ranges::indirectly_copyable<I, value_type*> graph(I first, S last) {
			std::for_each(first, last, [this](value_type v) {
				insert_node(v.from);
				insert_node(v.to);
				insert_edge(v.from, v.to, v.weight);
			});
		}

		// move constructor
		graph(graph&& other) noexcept
		: node_list_{std::move(other.node_list_)}
		, edge_list_{std::move(other.edge_list_)} {
			other.node_list_.clear();
			other.edge_list_.clear();
		}

		// move assignment
		auto operator=(graph&& other) noexcept -> graph& {
			node_list_ = std::move(other.node_list_);
			edge_list_ = std::move(other.edge_list_);
			other.node_list_.clear();
			other.edge_list_.clear();
			return *this;
		}
		// copy constructor
		graph<N, E>(graph const&) = default;

		// copy assignment
		auto operator=(graph const& other) -> graph& {
			if (this != &other) {
				auto copy = graph(other);
				std::swap(copy, *this);
				return *this;
			}
			return *this;
		}

		// =============================
		// MODIFIERS (spec: section 2.3)
		// -----------------------------

		// modifier 1 (inserting a node)
		template<typename T>
		auto insert_node(T const new_node) -> bool {
			if (!is_node(new_node)) {
				return node_list_.emplace(std::make_shared<node>(new_node)).second;
			}
			return false;
		}
		// modifier 2 (inserting an edge)
		template<typename T, typename U>
		auto insert_edge(T f, T t, U w) -> bool {
			if (!is_node(f) or !is_node(t)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::insert_edge when either src "
				                         "or dst node does not exist");
			}
			return edge_list_.emplace(std::make_shared<edge>(find_node(f), find_node(t), w)).second;
		}

		// modifier 3 (replacing a node)
		auto replace_node(N const& old_data, N const& new_data) -> bool {
			if (is_node(new_data)) {
				return false;
			}
			if (is_node(old_data)) {
				(*node_list_.find(old_data)).get()->set_node_value(new_data);
			}
			else {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::replace_node on a node "
				                         "that doesn't exist");
			}
			return true;
		}

		// modifier 4 (replacing a node and redirect weights to new node)
		auto merge_replace_node(N const& old_data, N const& new_data) -> void {
			if (!is_node(new_data) or !is_node(old_data)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::merge_replace_node on old "
				                         "or new data if they don't exist in the graph");
			}
			// get rid of old node
			node_list_.erase(find_node(old_data));

			if (edge_list_.empty()) {
				return; // no edges - no point looking for them
			}
			// reroute edges
			for (auto it_edge_ptr : edge_list_) {
				if (it_edge_ptr->get_from_node() == old_data) {
					it_edge_ptr->set_from_ptr(find_node(new_data));
				}
				if (it_edge_ptr->get_to_node() == old_data) {
					it_edge_ptr->set_to_ptr(find_node(new_data));
				}
			}

			// get rid of duplicate edges
			remove_duplicate_edges();
		}

		// modifier 5 (erase node and edges from and to that node)
		auto erase_node(N const& value) noexcept -> bool {
			if (!is_node(value)) {
				return false;
			}
			if (node_list_.erase(find_node(value))) { // only remove edges if node is deleted
				// collect pointers to edges that need to be removed
				auto edges_2_delete = std::vector<std::shared_ptr<edge>>{};
				for (auto it_edge_ptr : edge_list_) {
					if (it_edge_ptr->get_from_node() == value) {
						edges_2_delete.push_back(it_edge_ptr);
					}
					if (it_edge_ptr->get_to_node() == value) {
						edges_2_delete.push_back(it_edge_ptr);
					}
				}
				//  delete those edges
				for (auto edge_ptr : edges_2_delete) {
					edge_list_.erase(edge_ptr);
				}
				return true;
			}
			return false;
		}

		// modifier 6 (remove an edge from the graph - with node/node/weight)
		auto erase_edge(N const& src, N const& dst, E const& weight) -> bool {
			if (!is_node(src) or !is_node(dst)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::erase_edge on src or dst "
				                         "if they don't exist in the graph");
			}
			return (is_edge(src, dst, weight) // edge exists and it can be deleted
			        and edge_list_.erase(find_edge(src, dst, weight)));
		}

		// modifier 7 (remove an edge from graph - with an iterator)
		auto erase_edge(iterator& it) -> iterator {
			if (it == this->end()) {
				return it; // no edge to remove
			}
			auto this_edge = get_value_type(it);
			auto it_cpy = it;
			auto next_edge = ++it_cpy != end() ? get_value_type(it_cpy) : this_edge;
			erase_edge(this_edge.from, this_edge.to, this_edge.weight);
			return this_edge == next_edge ? end() : find(next_edge.from, next_edge.to, next_edge.weight);
		}

		// modifier 8 (erases a range of edges)
		auto erase_edge(iterator& i, iterator& s) -> iterator {
			if ((i == end()) or (i == s)) {
				return i; // nothing to do
			}

			auto start_edge = get_value_type(i);
			auto end_edge = s != end() ? get_value_type(s) : start_edge;
			edge_list_.erase(edge_list_.find(start_edge), edge_list_.find(end_edge));
			return start_edge == end_edge ? end() : find(end_edge.from, end_edge.to, end_edge.weight);
		}

		// modifier 9 (erases all nodes and edges from graph)
		auto clear() noexcept -> void {
			edge_list_.clear();
			node_list_.clear();
		}

		// =======================
		// ACCESSORS (section 2.4)
		// -----------------------
		// accessor 1 (checks if a value represents a node)
		[[nodiscard]] auto is_node(N n) -> bool {
			return node_list_.find(n) != node_list_.end();
		}

		// accessor 2 (checks if the graph is empty
		[[nodiscard]] auto empty() -> bool {
			return node_list_.empty();
		}

		// accessor 3 (checks if two nodes are connected)
		[[nodiscard]] auto is_connected(N const& src, N const& dst) -> bool {
			if (!is_node(src) or !is_node(dst)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::is_connected if src or dst "
				                         "node don't exist in the graph");
			}
			auto it1 = edge_list_.lower_bound(value_type{src, dst, std::numeric_limits<E>::min()});
			return ((it1 != edge_list_.end()) and ((*it1)->get_from_node() == src)
			        and ((*it1)->get_to_node() == dst));
		}

		// accessor 4 (returns a sequence of nodes
		[[nodiscard]] auto nodes() -> std::vector<N> {
			auto node_sequence = std::vector<N>{};
			for (auto node_ptr : node_list_) {
				node_sequence.push_back(node_ptr->get_node_value());
			}
			return node_sequence;
		}

		// accessor 5 (returns a sequence of weights)
		[[nodiscard]] auto weights(N const& from, N const& to) -> std::vector<E> {
			auto weights_sequence = std::vector<E>{};
			if (!is_node(from) or !is_node(to)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::weights if src or dst node "
				                         "don't exist in the graph");
			}
			auto w_it = edge_list_.lower_bound(value_type{from, to, std::numeric_limits<E>::min()});
			while ((w_it != edge_list_.end()) and ((*w_it).get()->get_to_node() == to)) {
				weights_sequence.push_back((*w_it).get()->get_edge_weight());
				++w_it;
			}
			return weights_sequence;
		}
		// accessor 6 (return an iterator to an edge)
		[[nodiscard]] auto find(N const& src, N const& dst, E const& weight) -> iterator {
			return iterator(edge_list_, edge_list_.find(value_type{src, dst, weight}));
		}
		// accessor 7 (returns a sequence of nodes connected to a given node)
		[[nodiscard]] auto connections(N const& src) -> std::vector<N> {
			auto connections = std::vector<N>{};
			if (!is_node(src)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::connections if src doesn't "
				                         "exist in the graph");
			}
			auto c_it = edge_list_.lower_bound(
			   value_type{src, std::numeric_limits<N>::min(), std::numeric_limits<E>::min()});
			while (c_it != edge_list_.end() and (*c_it).get()->get_from_node() == src) {
				connections.push_back((*c_it).get()->get_to_node());
				++c_it;
			}
			return connections;
		}

		// ==========================
		// RANGE ACCESS (section 2.5)
		// --------------------------

		// range access 1 (return iterator to first element in graph)
		[[nodiscard]] auto begin() const -> iterator {
			// std::set<std::shared_ptr<edge>, edge_comparator> const& temp = edge_list_;
			return iterator(edge_list_, edge_list_.begin());
		}

		// range access 2 (return iterator to end of the range of elements
		[[nodiscard]] auto end() const -> iterator {
			return iterator(edge_list_, edge_list_.end());
		}

		// =========================
		// COMPARISONS (section 2.6)
		// -------------------------

		[[nodiscard]] auto operator==(graph const& other) const -> bool {
			// check same number of nodes and edges
			if ((this->node_list_.size() != other.node_list_.size())
			    or (this->edge_list_.size() != other.edge_list_.size()))
			{
				return false;
			}
			// check if nodes are identical
			auto n_it1 = this->node_list_.begin();
			auto n_it2 = other.node_list_.begin();
			while ((n_it1 != this->node_list_.end())
			       and ((*n_it1)->get_node_value() == (*n_it2)->get_node_value()))
			{
				++n_it1;
				++n_it2;
			}

			if (n_it1 != this->node_list_.end()) {
				return false; // node lists aren't the same
			}
			auto e_it1 = this->edge_list_.begin();
			auto e_it2 = other.edge_list_.begin();
			while ((e_it1 != this->edge_list_.end())
			       and ((*e_it1)->get_edge_details() == (*e_it2)->get_edge_details()))
			{
				++e_it1;
				++e_it2;
			}

			return (e_it1 == this->edge_list_.end()); // edge lists aren't the same
		}

		// ========================
		// EXTRACTOR (section 2.7)
		// ------------------------

		auto friend operator<<(std::ostream& os, graph const& g) -> std::ostream& {
			for (auto node_ptr : g.node_list_) {
				os << fmt::format("{} (\n", node_ptr->get_node_value());
				for (auto edge_ptr : g.edge_list_) {
					if (edge_ptr->get_from_node() == node_ptr->get_node_value()) {
						os << fmt::format("  {} | {}\n",
						                  edge_ptr->get_to_node(),
						                  edge_ptr->get_edge_weight());
					}
				}
				os << ")\n";
			}
			return os;
		}

		// ===========================
		//  FUNCTIONS FOR TESTING ONLY
		// ---------------------------
		auto print_vals_test() {
			for (auto it = node_list_.begin(); it != node_list_.end(); it++) {
				std::cout << "The value is: " << it->get()->get_node_value() << std::endl;
				for (auto it2 = this->edge_list_.begin(); it2 != this->edge_list_.end(); it2++) {
					if (it2->get()->get_from_node() == it->get()->get_node_value()) {
						std::cout << "The edge is: " << it2->get()->get_edge_weight() << std::endl;
					}
				}
			}
		}
		auto print_edges_test() {
			for (auto it = edge_list_.begin(); it != edge_list_.end(); it++) {
				std::cout << it->get()->get_from_node() << "(" << it->get()->get_from_count() << ")"
				          << " -> " << it->get()->get_to_node() << " -> "
				          << it->get()->get_edge_weight() << std::endl;
			}
		}

		auto edge_details_test(value_type const& e) -> std::string {
			// returns a string  from_node (ptr_count) | weight | to_node (ptr_count)
			auto return_string = std::string{};
			auto it = edge_list_.find(e);
			if (it != edge_list_.end()) {
				return_string = fmt::format("{}({}) | {} | {}({}) ",
				                            (*it).get()->get_from_node(),
				                            (*it).get()->get_from_count(),
				                            (*it).get()->get_edge_weight(),
				                            (*it).get()->get_to_node(),
				                            (*it).get()->get_to_count());
			}
			return return_string;
		}

		auto test_edge_details(E e) -> std::string {
			// returns a string  from_node (ptr_count) | weight | to_node (ptr_count)
			auto return_string = std::string{};
			auto it = edge_list_.find(e); // find first occurance
			while (it != edge_list_.end()) {
				return_string = fmt::format("{}{}({}) | {} | {}({}) ",
				                            return_string,
				                            (*it).get()->get_from_node(),
				                            (*it).get()->get_from_count(),
				                            (*it).get()->get_edge_weight(),
				                            (*it).get()->get_to_node(),
				                            (*it).get()->get_to_count());
				it++; // could be another one
			}
			return return_string;
		}

		// Helper/utility functions

	private:
		auto find_node(N n) -> std::shared_ptr<node> const& {
			return *node_list_.find(n);
		}
		auto find_edge(N const& src, N const& dst, E const& weight) -> std::shared_ptr<edge> const& {
			return *edge_list_.find(value_type{src, dst, weight});
		}
		auto find_edge(value_type e) -> std::shared_ptr<edge> const& {
			return *edge_list_.find(e);
		}
		auto get_value_type(iterator& it) -> value_type {
			return (value_type{std::get<0>(*it), std::get<1>(*it), std::get<2>(*it)});
		}
		auto remove_duplicate_edges() {
			auto edges_to_delete = std::vector<value_type>{};
			auto it_edge_ptr = edge_list_.begin();
			auto prev = (*it_edge_ptr++)->get_edge_details();
			while (it_edge_ptr != edge_list_.end()) {
				if ((*it_edge_ptr)->get_edge_details() == prev) {
					edges_to_delete.push_back(prev);
				}
				++it_edge_ptr;
			}
			for (auto i : edges_to_delete) {
				erase_edge(i.from, i.to, i.weight);
			}
		}

		// auto find_weight(N const& src, N const& dst) -> E {
		// return ()

		// }

		[[nodiscard]] auto is_edge(N const& src, N const& dst, E const& weight) -> bool {
			return edge_list_.find(value_type{src, dst, weight}) != edge_list_.end();
		}
		// [[nodiscard]] auto is_edge(value_type e) -> bool {
		// 	return edge_list_.find(e) != edge_list_.end();
		// }

		// COMPARATORS
		struct node_comparator {
			using is_transparent = std::true_type;

			auto operator()(std::shared_ptr<node> const& x, std::shared_ptr<node> const& y) const
			   -> bool {
				return x->get_node_value() < y->get_node_value();
			}
			auto operator()(std::shared_ptr<node> const& x, N const& y) const -> bool {
				return x->get_node_value() < y;
			}
			auto operator()(N const& x, std::shared_ptr<node> const& y) const -> bool {
				return x < y->get_node_value();
			}
		};
		struct edge_comparator {
			using is_transparent = std::true_type;
			auto operator()(std::shared_ptr<edge> const& x, std::shared_ptr<edge> const& y) const
			   -> bool {
				if (x->get_from_node() != y->get_from_node()) {
					return x->get_from_node() < y->get_from_node();
				}
				if (x->get_to_node() != y->get_to_node()) {
					return x->get_to_node() < y->get_to_node();
				}
				return x->get_edge_weight() < y->get_edge_weight();
			}
			auto operator()(std::shared_ptr<edge> const& x, value_type const& y) const -> bool {
				if (x->get_from_node() != y.from) {
					return x->get_from_node() < y.from;
				}
				if (x->get_to_node() != y.to) {
					return x->get_to_node() < y.to;
				}
				return x->get_edge_weight() < y.weight;
			}
			auto operator()(value_type const& x, std::shared_ptr<edge> const& y) const -> bool {
				if (x.from != y->get_from_node()) {
					return x.from < y->get_from_node();
				}
				if (x.to != y->get_to_node()) {
					return x.to < y->get_to_node();
				}
				return x.weight < y->get_edge_weight();
			}
		};

		std::set<std::shared_ptr<node>, node_comparator> node_list_{}; // NODE LIST (SET)
		std::set<std::shared_ptr<edge>, edge_comparator> edge_list_{}; // EDGE LIST (SET)

	public:
	}; // namespace gdwg

	//   ITERATOR CLASS
	//   --------------
	template<concepts::regular N, concepts::regular E>
	requires concepts::totally_ordered<N> //
	   and concepts::totally_ordered<E> //

	   class graph<N, E>::iterator {
	public:
		using value_type = ranges::common_tuple<N, N, E>;
		using difference_type = std::ptrdiff_t;
		using iterator_category = std::bidirectional_iterator_tag;
		using graph_iterator = typename std::set<std::shared_ptr<edge>>::const_iterator;

		// iterator constructor
		iterator() = default;
		explicit iterator(std::set<std::shared_ptr<edge>, edge_comparator> const& edge_list,
		                  graph_iterator const& it)
		: edge_list_ref_(edge_list)
		, iterator_(it) {}

		// iterator source
		auto operator*() -> ranges::common_tuple<N const&, N const&, E const&> {
			using graph_tuple = ranges::common_tuple<N const&, N const&, E const&>;
			return graph_tuple{std::make_tuple((*iterator_).get()->get_from_node(),
			                                   (*iterator_).get()->get_to_node(),
			                                   (*iterator_).get()->get_edge_weight())};
		}

		// iterator traversal
		auto operator++() -> iterator& {
			++iterator_;
			return *this;
		}

		auto operator++(int) -> iterator {
			auto temp = *this;
			++*this;
			return temp;
		}

		auto operator--() -> iterator& {
			--iterator_;
			return *this;
		}

		auto operator--(int) -> iterator {
			auto temp = *this;
			--*this;
			return temp;
		}

		// iterator comparison
		auto operator==(iterator const& other) const -> bool {
			return (this->iterator_ == other.iterator_);
		}

	private:
		// explicit iterator(unspecified);
		std::set<std::shared_ptr<edge>, edge_comparator> const& edge_list_ref_;
		graph_iterator iterator_;
	}; // end of interator

	template<concepts::regular N, concepts::regular E>
	requires concepts::totally_ordered<N> //
	   and concepts::totally_ordered<E> //
	   auto operator<<(std::ostream& os, graph<N, E> const& g) -> std::ostream& {
		(void)g;
		return os;
	}

} // namespace gdwg

#endif // GDWG_GRAPH_HPP
