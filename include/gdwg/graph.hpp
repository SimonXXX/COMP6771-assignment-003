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
		struct value_type { // edges
			N from;
			N to;
			E weight;
			[[nodiscard]] auto operator==(value_type const& other) const -> bool {
				return static_cast<bool>((this->from == other.from) and (this->to == other.to)
				                         and (this->weight == other.weight));
			}
		};

		// =================
		// = INNER CLASSES =
		// =================
		//
		class iterator;

		//   NODE CLASS
		//   ----------
		class node {
		public:
			// node constructors
			node() {
				node_value_ = N{};
			}
			explicit node(N val) {
				node_value_ = val;
			}
			// node setters
			void set_node_value(N val) {
				node_value_ = val;
			}
			// node getters
			[[nodiscard]] auto get_node_value() -> N {
				return node_value_;
			}

		private:
			N node_value_{};
		}; // end of node

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

		// ====================
		// END OF INNER CLASSES
		// ====================
		//

		// ================================
		// CONSTRUCTORS (spec: section 2.2)
		// --------------------------------

		graph<N, E>() = default;

		// explicit graph<N, E>(N val) {
		// 	insert_node(val);
		// }

		graph(std::initializer_list<N> il) {
			std::for_each(il.begin(), il.end(), [this](N const& n) { insert_node(n); });
		}

		template<ranges::forward_iterator I, ranges::sentinel_for<I> S>
		requires ranges::indirectly_copyable<I, N*> graph<N, E>(I first, S last) {
			std::for_each(first, last, [this](N const& n) { insert_node(n); });
		}

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

		graph<N, E>(graph const&) = default;

		// =============================
		// MODIFIERS (spec: section 2.3)
		// -----------------------------

		template<typename T>
		auto insert_node(T const new_node) {
			if (!is_node(new_node)) {
				node_list_.emplace(std::make_shared<node>(new_node));
			}
		}

		template<typename T, typename U>
		auto insert_edge(T f, T t, U w) -> bool {
			return edge_list_.emplace(std::make_shared<edge>(find_node(f), find_node(t), w)).second;
		}
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

		auto merge_replace_node(N const& old_data, N const& new_data) -> void {
			if (!is_node(new_data) or !is_node(old_data)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::merge_replace_node on old "
				                         "or new data if they don't exist in the graph");
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
			node_list_.erase(find_node(old_data)); // get rid of old node
			// Effects: The node equivalent to old_data in the graph are replaced with instances
			// of new_data. After completing, every incoming and outgoing edge of old_data
			// becomes an incoming/ougoing edge of new_data, except that duplicate edges shall be
			// removed.
			// Postconditions: All iterators are invalidated.
			// Throws: std::runtime_error("Cannot call gdwg::graph<N, E>::merge_replace_node on old
			// or new data if they don't exist in the graph") if either of is_node(old_data) or
			// is_node(new_data) are false.
		}
		auto erase_node(N const& value) -> bool {
			if (!is_node(value)) {
				return false;
			}
			auto edges_2_delete = std::vector<std::shared_ptr<edge>>{};
			for (auto it_edge_ptr : edge_list_) {
				if (it_edge_ptr->get_from_node() == value) {
					edges_2_delete.push_back(it_edge_ptr);
				}
				if (it_edge_ptr->get_to_node() == value) {
					edges_2_delete.push_back(it_edge_ptr);
				}
			}
			for (auto edge_ptr : edges_2_delete) {
				edge_list_.erase(edge_ptr);
			}

			return node_list_.erase(find_node(value));
		}
		// Effects: Erases all nodes equivalent to value, including all incoming and
		// outgoing edges.
		// Complexity: O(log (n) + e), where n is the total number of stored nodes
		// and e is the total number of stored edges.
		// Returns: true if value was removed; false
		// otherwise.
		// Postconditions: All iterators are invalidated.

		auto erase_edge(N const& src, N const& dst, E const& weight) -> bool {
			if (!is_node(src) or !is_node(dst)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::erase_edge on src or dst "
				                         "if "
				                         "they don't exist in the graph");
			}
			if (!is_edge(value_type{src, dst, weight})) {
				return false;
			}
			return edge_list_.erase(find_edge(src, dst, weight));
		};
		// 		Effects: Erases an edge representing src → dst with weight weight.
		// Returns: true if an edge was removed; false otherwise.
		// Postconditions: All iterators are invalidated.
		// Throws: std::runtime_error("Cannot call gdwg::graph<N, E>::erase_edge on src or dst if
		// they don't exist in the graph") if either is_node(src) or is_node(dst) is false.
		// Complexity: O(log (n) + e), where n is the total number of stored nodes and e is the
		// total number of stored edges.
		auto erase_edge(iterator i) -> iterator;
		// Effects: Erases the edge pointed to by i.
		// Complexity: Amortised constant time.
		// Returns: An iterator pointing to the element immediately after i prior to the element
		// being erased. If no such element exists, returns end(). Postconditions: All iterators
		// are invalidated. [Note: The postcondition is slightly stricter than a real-world
		// container to help make the assingment easier (i.e. we won’t be testing any iterators
		// post-erasure). —end note]
		auto erase_edge(iterator i, iterator s) -> iterator;
		// Effects: Erases all edges in the range [i, s).
		// Complexity O(d), where d=ranges::distance(i, s).
		// Returns: An iterator equivalent to s prior to the range being erased. If no such element
		// exists, returns end(). Postconditions: All iterators are invalidated. [Note: The
		// postcondition is slightly stricter than a real-world container to help make the
		// assingment easier (i.e. we won’t be testing any iterators post-erasure). —end note]
		auto clear() noexcept -> void {
			edge_list_.clear();
			node_list_.clear();
		}
		// Effects: Erases all nodes from the graph.
		// Postconditions: empty() is true

		// =======================
		// ACCESSORS (section 2.4)
		// -----------------------

		[[nodiscard]] auto is_node(N n) -> bool {
			return node_list_.find(n) != node_list_.end();
		}
		[[nodiscard]] auto is_edge(N const& src, N const& dst, E const& weight) -> bool {
			return edge_list_.find(value_type{src, dst, weight}) != edge_list_.end();
		}
		[[nodiscard]] auto is_edge(value_type e) -> bool {
			return edge_list_.find(e) != edge_list_.end();
		}
		[[nodiscard]] auto empty() -> bool {
			return node_list_.empty();
		}

		[[nodiscard]] auto is_connected(N const& src, N const& dst) -> bool {
			if (!is_node(src) or !is_node(dst)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::is_connected if src or dst "
				                         "node don't exist in the graph");
			}
			auto it1 = edge_list_.lower_bound(value_type{src, dst, std::numeric_limits<E>::min()});
			return ((it1 != edge_list_.end()) and ((*it1)->get_from_node() == src)
			        and ((*it1)->get_to_node() == dst));
		}

		[[nodiscard]] auto nodes() -> std::vector<N> {
			auto node_sequence = std::vector<N>{};
			for (auto node_ptr : node_list_) {
				node_sequence.push_back(node_ptr->get_node_value());
			}
			return node_sequence;
		}
		[[nodiscard]] auto weights(N const& from, N const& to) -> std::vector<E> {
			auto weights_sequence = std::vector<E>{};
			if (!is_node(from) or !is_node(to)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::weights if src or dst node "
				                         "don't "
				                         "exist in the graph");
			}
			auto w_it = edge_list_.lower_bound(value_type{from, to, std::numeric_limits<E>::min()});
			while ((w_it != edge_list_.end()) and ((*w_it).get()->get_to_node() == to)) {
				weights_sequence.push_back((*w_it).get()->get_edge_weight());
				++w_it;
			}
			return weights_sequence;
		}

		[[nodiscard]] auto find(N const& src, N const& dst, E const& weight) -> iterator {
			return iterator(edge_list_, edge_list_.find(value_type{src, dst, weight}));
		}

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
		// Returns: A sequence of nodes (found from any immediate outgoing edge) connected to
		// src, sorted in ascending order, with respect to the connected nodes.
		// Complexity: O(log??(n)??+??e), where e is the number of outgoing edges associated with
		// src.

		// ==========================
		// RANGE ACCESS (section 2.5)
		// --------------------------

		[[nodiscard]] auto begin() const -> iterator {
			// std::set<std::shared_ptr<edge>, edge_comparator> const& temp = edge_list_;
			return iterator(edge_list_, edge_list_.begin());
		}
		[[nodiscard]] auto end() const -> iterator {
			return iterator(edge_list_, edge_list_.end());
		}

		// ========================
		// COMPARISONS(section 2.6)
		// ------------------------

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

		// for (it1 = this->node_list_.begin(), it2 = other.node_list_.begin();
		//      ((it1 == this->node_list_.end())
		//       or ((*it1)->get_node_value() != (*it2)->get_node_value()));
		//      ++it1, ++it2)
		// {
		// };
		// for (auto i : zip(this->node_list_, other.node_list_)) {
		// 	if (std::get<0>(i) != std::get<1>(i)->get_node_value()) {
		// 		return false;
		// 	}
		// }
		// check if edges are identical
		// for (auto i : zip(this->edge_list_, other.edge_list_)) {
		// 	if (std::get<0>(i)->get_edge_details() != std::get<1>(i)->get_edge_details()) {
		// 		return false;
		// 	}
		// }
		//}

		// Returns: true if *this and other contain exactly the same nodes and edges, and false
		// otherwise. Complexity: O(n??+??e) where n is the sum of stored nodes in *this and other,
		// and e is the sum of stored edges in *this and other.

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

		// ==============================
		//  FUNCTIONS FOR TESTING ONLY  |
		// ------------------------------
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
				          << " -> " << it->get()->get_edge_weight() << " -> "
				          << it->get()->get_to_node() << std::endl;
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
		// iterator() = default;
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
