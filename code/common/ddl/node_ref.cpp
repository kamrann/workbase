// node_ref.cpp

#include "node_ref.h"
#include "values/value.h"
#include "values/navigator.h"


namespace ddl {

	navigator nav_to_ref(node_ref const& ref, navigator nav)
	{
		auto rf = resolve_reference_unique(ref, nav);
		if(rf)
		{
			return navigator{ nav.tree_, rf };
		}
		else
		{
			return{};
		}
	}

	sd_node_ref_list resolve_reference(node_ref const& ref, navigator nav)
	{
		if(nav)
		{
			auto id = ref.nd.get_id();
			auto fr = nav.find_by_id(id);	// todo: perhaps this call will need to be passed info from node_ref::res context settings

			if(!fr.by_location.empty())
			{
				if(ref.res == node_ref::Resolution::Unique)
				{
					auto const& first_list = fr.by_location.begin()->second;
					if(first_list.size() == 1)
					{
						auto const& pth = first_list.front();
						auto rnav = nav[pth];
						return{ rnav.get_ref() };
					}
					else
					{
						throw std::runtime_error("ambiguous reference id lookup");
					}
				}
				else // todo: more fine grained control of resolving context done here...
						// currently just accept any matching id anywhere in hierarchy
				{
					sd_node_ref_list results;
					for(auto const& entry : fr.by_location)
					{
						auto const& res_list = entry.second;
						for(auto const& pth : res_list)
						{
							auto rnav = nav[pth];
							results.insert(rnav.get_ref());
						}
					}
					return results;
				}
			}
		}

		return{};
	}

	sd_node_ref resolve_reference_unique(node_ref const& ref, navigator nav)
	{
		auto res = resolve_reference(ref, nav);
		if(res.size() != 1)
		{
			throw std::runtime_error("ambiguous reference id lookup");
		}
		else
		{
			return *res.begin();
		}
	}

	sd_node_ref unique_reference(sd_node_ref_list const& list)
	{
		if(list.size() == 1)
		{
			return *list.begin();
		}
		else
		{
			return{};
		}
	}

}




