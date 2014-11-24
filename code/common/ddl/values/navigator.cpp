// navigator.cpp

#include "navigator.h"


namespace ddl {

	navigator::navigator():
		node_{},
		deactivated_{ false }
	{

	}

	navigator::navigator(value_node nd):
		node_{ nd },
		deactivated_{ false }
	{

	}

	navigator::navigator(value_node nd, std::stack< value_node > anc, path pth):
		node_{ nd },
		ancestor_stk_{ std::move(anc) },
		path_{ pth },
		deactivated_{ false }
	{

	}

	navigator::operator bool() const
	{
		return node_;
	}

	bool navigator::is_parent() const
	{
		return !ancestor_stk_.empty();
	}

	navigator navigator::parent() const
	{
		auto anc = ancestor_stk_;
		auto nd = anc.top();
		anc.pop();
		auto p = path_;
		p.pop();
		return navigator{ nd, anc, p };
	}

	bool navigator::at_leaf() const
	{
		return node_.is_leaf();
	}

	path navigator::where() const
	{
		return path_;
	}

	bool navigator::is_child(node_name child_id) const
	{
		if(!node_.is_composite())
		{
			return false;
		}
		auto comp = node_.as_composite();
		return comp.find(child_id) != comp.end();
	}

	navigator navigator::child(node_name child_id) const
	{
		auto anc = ancestor_stk_;
		anc.push(node_);
		auto p = path_;
		p += child_id;
		return navigator{ node_.as_composite().find(child_id)->second, anc, p };
	}
	
	navigator navigator::operator[] (node_name child_id) const
	{
		return child(child_id);
	}

	navigator navigator::operator[] (char const* const child_id) const
	{
		return child(child_id);
	}

	std::vector< node_name > navigator::child_names() const
	{
		if(!node_.is_composite())
		{
			return{};
		}

		auto comp = node_.as_composite();
		std::vector< node_name > names;
		for(auto const& entry : comp)
		{
			names.push_back(entry.first);
		}
		return names;
	}

	size_t navigator::list_num_items() const
	{
		return node_.as_list().size();
	}

	navigator navigator::operator[] (size_t item_idx) const
	{
		auto anc = ancestor_stk_;
		anc.push(node_);
		auto p = path_;
		p += item_idx;
		return navigator{ node_.as_list()[item_idx], anc, p };
	}

	navigator navigator::deactivated() const
	{
		auto nav = *this;
		nav.deactivated_ = true;
		return nav;
	}

	bool navigator::is_active() const
	{
		return !deactivated_;
	}

	value_node navigator::get() const
	{
		return node_;
	}

	navigator navigator::operator[] (path const& pth) const
	{
		navigator nav{ *this };
		while(nav.is_parent())
		{
			nav = nav.parent();
		}

		for(auto const& cmp : pth)
		{
			switch(cmp.type())
			{
				case path::ComponentType::CompositeChild:
				nav = nav[cmp.as_child()];
				break;

				case path::ComponentType::ListItem:
				nav = nav[cmp.as_index()];
				break;
			}
		}

		return nav;
	}


/*	ref_resolver navigator::get_ref_resolver() const
	{
		return[](node_ref const&) -> value_node
		{

		};
	}
*/

	// TODO: Currently doesn't check for duplicates
	navigator::find_results& navigator::find_results::operator|= (find_results const& rhs)
	{
		for(auto const& entry : rhs.by_location)
		{
			auto const& loc = entry.first;
			auto& result_list = by_location[loc];
			result_list.insert(
				std::end(result_list),
				std::begin(entry.second),
				std::end(entry.second)
				);
		}
		return *this;
	}

	navigator::find_results navigator::find_results::operator| (find_results const& rhs) const
	{
		auto result = *this;
		result |= rhs;
		return result;
	}


	navigator::find_results::path_list navigator::find_descendent_r_id(node_id id) const
	{
		if(node_.is_composite())
		{
			find_results::path_list results;
			auto cnames = child_names();
			for(auto const& cn : cnames)
			{
				auto cnav = child(cn);
				auto c_id = cnav.get().get_id();
				if(c_id == id)
				{
					results.push_back(cnav.where());
				}

				auto from_child = cnav.find_descendent_r_id(id);
				results.insert(
					std::end(results),
					std::begin(from_child),
					std::end(from_child)
					);
			}
			return results;
		}
		else if(node_.is_list())
		{
			find_results::path_list results;
			auto count = list_num_items();
			for(size_t idx = 0; idx < count; ++idx)
			{
				auto cnav = (*this)[idx];
				auto c_id = cnav.get().get_id();
				if(c_id == id)
				{
					results.push_back(cnav.where());
				}

				auto from_child = cnav.find_descendent_r_id(id);
				results.insert(
					std::end(results),
					std::begin(from_child),
					std::end(from_child)
					);
			}
			return results;
		}
		else
		{
			return{};
		}
	}

	navigator::find_results navigator::find_by_id(node_id id) const
	{
		find_results results;
		
		auto root = (*this)[path{}];
		auto all = root.find_descendent_r_id(id);
		auto here = where();
		for(auto const& pth : all)
		{
			auto common_anc = path::common_ancestor(pth, here);
			if(common_anc == here)
			{
				// TODO: for now auto excluding descendents - specify this option as method argument
				// disabled		continue;
				
				// !!!!!!!!!!!!
				// Not sure how to deal with this - exclusion was since it doesn't make sense when resolving
				// references to look in descendents, since a node should never be dependent on the value of
				// one of its own descendents (pretty sure about this).
				// However, due to untestable conditional nodes resulting in no node whatsoever, when evaluating
				// a condition, the navigator cannot move to the corresponding place since as yet it may not
				// exist. In which case, it becomes deactivated and remains at the parent location. Therefore,
				// with lookup occurring from there, a node which is not a descendent of the conditional node
				// (and therefore is a valid dependency) may in fact be a descendent from the point of view
				// of the navigator location - ie. a dependency on a sibling.
				// Currently disabling descendent exclusion here, but may instead want another solution,
				// possibly involving keeping track (maybe within the navigator) of intended location (which may
				// not exist) as well as actual location.
			}

			auto up = here.length() - common_anc.length();
			auto down = pth.length() - common_anc.length();

			results.by_location[find_results::result_location{ up, down }].push_back(pth);
		}

		/*
		// TODO: For now, ignore this node itself, and decendents
		// In fact, just check siblings for now
		if(is_parent())
		{
			auto nav = parent();
			auto children = nav.child_names();
			for(auto const& name : children)
			{
				auto cnav = nav[name];
				if(cnav.get().get_id() == id)
				{
					find_results::result_location loc;
					loc.up_count = 1;
					loc.down_count = 1;
					results.by_location[loc].push_back(cnav.where());
					// TODO: just assuming only 1 matching
					break;
				}
			}
		}
		*/

		return results;
	}

	navigator::find_results::path_list navigator::find_child(node_name str/*, find_results::result_location offset*/) const
	{
		if(!*this || !get().is_composite())
		{
			return{};
		}

		find_results::path_list results;
//		auto loc = find_results::result_location{ offset.up_count, offset.down_count + 1 };
		auto cnames = child_names();
		for(auto const& cn : cnames)
		{
			if(cn == str)
			{
				results.push_back(child(cn).where());
			}
		}
		return results;
	}

	navigator::find_results::path_list navigator::find_descendent_r(node_name str/*, find_results::result_location offset*/) const
	{
		if(node_.is_composite())
		{
			find_results::path_list results = find_child(str/*, offset*/);
//			offset.down_count += 1;
			auto cnames = child_names();
			for(auto const& cn : cnames)
			{
				auto cnav = child(cn);
				auto from_child = cnav.find_descendent_r(str/*, offset*/);
				results.insert(
					std::end(results),
					std::begin(from_child),
					std::end(from_child)
					);
			}
			return results;
		}
		else if(node_.is_list())
		{
			return{};	// TODO:
		}
		else
		{
			return{};
		}
	}

	navigator::find_results navigator::find_descendent(node_name str) const
	{
		auto all = find_descendent_r(str);
		find_results results;
		auto here = where();
		for(auto const& pth : all)
		{
			auto depth = pth.length() - here.length();
			results.by_location[find_results::result_location{ 0, depth }].push_back(pth);
		}
		return results;
	}
	
	navigator::find_results navigator::find_ancestor(node_name str/*, find_results::result_location offset*/) const
	{
		navigator::find_results results;
/*		auto nav = *this;
		while(nav.is_parent())
		{

		}
*/		return results;
	}

	navigator::find_results navigator::find(node_name str) const
	{
		return{};	// TODO:
	}

}




