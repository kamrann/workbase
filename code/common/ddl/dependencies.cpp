// dependencies.cpp

#include "dependencies.h"
#include "values/navigator.h"


namespace ddl {

	dep_provider node_dependency(node_ref ref)
	{
		return[ref]()
		{
			return dep_list{ ref };
		};
	}


	// Combine all dependencies into one set
	dep_list dependent_component::deps() const
	{
		dep_list result;
		for(auto const& dp : deps_)
		{
			auto lst = dp();
			result.insert(std::begin(lst), std::end(lst));
		}
		return result;
	}
	
	dep_list dependent_component::operator() () const
	{
		return deps();
	}

	void dependent_component::add_dependency(dep_provider dp)
	{
		deps_.push_back(dp);
	}


/*	dep_function::dep_function()
	{}

	dep_function::dep_function(dep_function::function_t fn):
		fn_{ fn }
	{

	}

	dep_function::fn_result_t dep_function::operator() (navigator nav) const
	{
		return fn_(nav);
	}

	void dep_function::set_fn(function_t fn)
	{
		fn_ = fn;
	}
*/	
}


