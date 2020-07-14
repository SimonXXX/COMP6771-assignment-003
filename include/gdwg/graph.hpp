#ifndef GDWG_GRAPH_HPP
#define GDWG_GRAPH_HPP

#include <concepts/concepts.hpp>
#include <initializer_list>
#include <ostream>
#include <range/v3/iterator.hpp>
#include <range/v3/utility.hpp>

namespace gdwg {
	template<concepts::regular N, concepts::regular E>
	requires concepts::totally_ordered<N> //
	   and concepts::totally_ordered<E> //
	   class graph {
	public:
		class iterator;

		struct value_type {
			N from;
			N to;
			E weight;
		};

		// Your member functions go here
	private:
	};
} // namespace gdwg

#endif // GDWG_GRAPH_HPP
