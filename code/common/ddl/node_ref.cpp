// node_ref.cpp

#include "node_ref.h"
#include "values/value.h"
#include "values/navigator.h"


namespace ddl {

	navigator nav_to_ref(node_ref const& ref, navigator nav)
	{
		if(nav)
		{
			auto id = ref.nd.get_id();
			auto results = nav.find_by_id(id);	// todo: specify exclude descendents
			if(results.by_location.empty())
			{
				return{};
			}
			// TODO: this will need looking into. if the same defn is used multiple places in the tree,
			// we may resolve the wrong instance as a result of the one we are looking for (the closest)
			// currently not existing, due to, for example, it's parent node just having been activated
			// in a condition as a result of changing some other value, but not yet created.
			// In this case the desired result would be to fail the reference resolution, but chance that
			// it will instead resolve something else...
			auto first = results.by_location.begin()->second;
			if(first.size() > 1)
			{
				throw std::runtime_error("ambiguous reference id lookup");
			}

			auto pth = first.front();
			return nav[pth];
		}
		else
		{
			return{};
		}
	}

	value_node resolve_reference(node_ref const& ref, navigator nav)
	{
		nav = nav_to_ref(ref, nav);
		if(nav)
		{
			return nav.get();
		}
		else
		{
			return{};
		}
	}

}




