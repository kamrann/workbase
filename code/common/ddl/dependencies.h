// dependencies.h

#ifndef __WB_DDL_DEPENDENCIES_H
#define __WB_DDL_DEPENDENCIES_H

#include "node_ref.h"

#include <list>
#include <set>
#include <functional>
#include <memory>


namespace ddl {

	struct node_ref_compare
	{
		inline bool operator() (node_ref const& lhs, node_ref const& rhs) const
		{
			return lhs.nd.get_id() < rhs.nd.get_id();
		}
	};

	// Unrolled dependencies
	typedef std::set< node_ref, node_ref_compare > dep_list;

	// Function that provides unrolled dependencies
	typedef std::function< dep_list() > dep_provider;

	dep_provider node_dependency(node_ref ref);

	// Any object which can have dependencies should derive from this class
	class dependent_component
	{
	public:
		dep_list deps() const;
		dep_list operator() () const;

		void add_dependency(dep_provider dp);

	private:
		std::list< dep_provider > deps_;
	};


	/*/ this also a dependency?
	typedef std::map< std::string, node_ref > named_dep_map;
	typedef std::shared_ptr< named_dep_map > named_dep_map_ptr;
	*/

	// Need a nice easy way to create a named_dep_map from an arbitrary number of defn_nodes
	// The ptr is then captured by the lambda, and then the lambda and the ptr passed to dep_function constructor.

	// A class encapsulating a function (return value type?) which stores it's dependencies on other nodes.
	template < typename ResultType >
	class dep_function:
		public dependent_component
	{
	public:
//		typedef std::set< std::string > fn_result_t;
		typedef ResultType fn_result_t;
		typedef std::function< fn_result_t(navigator) > function_t;

	public:
		dep_function() {}
		dep_function(function_t fn):
			fn_{ fn }
		{}

	public:
		void set_fn(function_t fn)
		{
			fn_ = fn;
		}

		fn_result_t operator() (navigator nav) const
		{
			return fn_(nav);
		}

		using dependent_component::operator();

//		dep_list deps() const;

	private:
		function_t fn_;
//		named_dep_map_ptr named_deps_;
	};

}


#endif


