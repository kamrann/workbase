// param_accessor.cpp

#include "param_accessor.h"
#include "param_yaml.h"


namespace prm
{
	param_accessor::param_accessor(param_tree* pt):
		m_pt{ pt },
		m_locked{ false },
		m_auto_lock{ false },
		m_match_type{ Match::Exact },
		m_comp_type{ MatchComparison::Equal }
	{
		if(has_pt())
		{
			m_location_stk.push(m_pt->tree().get_root());
		}
	}

	qualified_path param_accessor::where() const
	{
		return path_from_node(m_location_stk.top());
	}

	bool param_accessor::at_leaf() const
	{
		return m_pt->tree().is_leaf(m_location_stk.top());
	}

	bool param_accessor::exists(qualified_path const& path) const
	{
		return node_from_path(path) != ptree_t::null_vertex();
	}

	bool param_accessor::is_available(std::string const& name) const
	{
		return find_node_unambiguous(name) != ptree_t::null_vertex();
	}

	param& param_accessor::operator[] (std::string const& name)
	{
		return *node_attribs(find_node_unambiguous(name)).value;
	}

	param param_accessor::operator[] (std::string const& name) const
	{
		return find_value(name);
	}

	// These overloads are needed to avoid ambiguity due to existence of operator bool()
	param& param_accessor::operator[] (char const* const name)
	{
		return operator[] (std::string{ name });
	}

	param param_accessor::operator[] (char const* const name) const
	{
		return operator[] (std::string{ name });
	}

	param_accessor::param_data& param_accessor::find_param(qualified_path const& path)
	{
		auto n = node_from_path(path);
		return node_attribs(n);
	}

	param_accessor::param_data param_accessor::find_param(qualified_path const& path) const
	{
		auto n = node_from_path(path);
		return node_attribs(n);
	}

	param_accessor::param_data param_accessor::find_param(std::string const& name) const
	{
		return node_attribs(find_node_unambiguous(name));
	}

	param param_accessor::find_value(qualified_path const& path) const
	{
		return *find_param(path).value;
	}

	param param_accessor::find_value(std::string const& name) const
	{
		return *node_attribs(find_node_unambiguous(name)).value;
	}

	param_accessor::param_data& param_accessor::param_here()
	{
		return node_attribs(m_location_stk.top());
	}

	param_accessor::param_data param_accessor::param_here() const
	{
		return node_attribs(m_location_stk.top());
	}

	param& param_accessor::value_here()
	{
		return *node_attribs(m_location_stk.top()).value;
	}

	param param_accessor::value_here() const
	{
		return *node_attribs(m_location_stk.top()).value;
	}

	qualified_path param_accessor::find_path(std::string const& name, indices_t const& indices) const
	{
		auto n = find_node_unambiguous(name, indices);
		return n == ptree_t::null_vertex() ? qualified_path{} : path_from_node(n);
	}

	qualified_path param_accessor::find_path_descendent(std::string const& name, indices_t const& indices) const
	{
		auto nodes = find_all_nodes(name, indices);
		for(auto it = std::begin(nodes); it != std::end(nodes);)
		{
			if(m_pt->tree().are_ancestor_descendent(m_location_stk.top(), *it))
			{
				++it;
			}
			else
			{
				it = nodes.erase(it);
			}
		}

		return nodes.size() == 1 ? path_from_node(*nodes.begin()) : qualified_path{};
	}

	qualified_path param_accessor::find_path_ancestor(std::string const& name) const
	{
		auto nodes = find_all_nodes(name);
		for(auto it = std::begin(nodes); it != std::end(nodes);)
		{
			if(m_pt->tree().are_ancestor_descendent(*it, m_location_stk.top()))
			{
				++it;
			}
			else
			{
				it = nodes.erase(it);
			}
		}

		return nodes.size() == 1 ? path_from_node(*nodes.begin()) : qualified_path{};
	}

	bool param_accessor::move_to(qualified_path const& abs)
	{
		if(is_locked())
		{
			return false;
		}

		auto n = node_from_path(abs);
		if(n == ptree_t::null_vertex())
		{
			if(m_auto_lock)
			{
				lock();
			}
			return false;
		}
		else
		{
			m_location_stk.push(n);
			return true;
		}
	}

	bool param_accessor::move_relative(qualified_path const& rel)
	{
		if(is_locked())
		{
			return false;
		}

		return move_to(where() + rel);
	}

	bool param_accessor::up()
	{
		if(is_locked())
		{
			return false;
		}

		auto parent = where();
		parent.pop();
		return move_to(parent);
	}

	bool param_accessor::revert()
	{
		if(is_locked())
		{
			return false;
		}

		if(m_location_stk.empty())
		{
			return false;
		}
		else
		{
			m_location_stk.pop();
			return true;
		}
	}

	void param_accessor::lock()
	{
		m_locked = true;
	}
		
	void param_accessor::unlock()
	{
		m_locked = false;
	}
	
	bool param_accessor::is_locked()
	{
		return m_locked;
	}
	
	void param_accessor::set_lock_on_failed_move(bool autolock)
	{
		m_auto_lock = autolock;
	}

	void param_accessor::set_match_method(Match match_type, MatchComparison comp_type)
	{
		m_match_type = match_type;
		m_comp_type = comp_type;
	}

	std::list< qualified_path > param_accessor::children() const
	{
		auto p = param_here();
		if(is_leaf_type(p.type))
		{
			return{};
		}

		std::list< qualified_path > clist;
		auto n = m_location_stk.top();
		auto& tree = m_pt->tree();
		auto crange = tree.children(n);
		for(auto c : crange)
		{
			clist.push_back(path_from_node(c));
		}
		return clist;
	}

	param_accessor::operator bool() const
	{
		return has_pt();
	}

	bool param_accessor::operator! () const
	{
		return !has_pt();
	}


	bool param_accessor::has_pt() const
	{
		return m_pt != nullptr;
	}

	// Assuming unique child names
	param_accessor::node_t param_accessor::child_by_name(node_t n, std::string const& name) const
	{
		auto& tree = m_pt->tree();
		auto children = tree.children(n);
		for(auto c : children)
		{
			if(tree[c].name == name)
			{
				return c;
			}
		}
		return ptree_t::null_vertex();
	}

	qualified_path param_accessor::path_from_node(node_t node) const
	{
		return m_pt->node_qpath(node);
	}

	param_accessor::node_t param_accessor::node_from_path(qualified_path const& p) const
	{
		if(!p)
		{
			return ptree_t::null_vertex();
		}

		auto& tree = m_pt->tree();
		auto n = tree.get_root();
		auto const length = p.size();
		for(size_t i = 0; i < length; ++i)
		{
			if(n == ptree_t::null_vertex())
			{
				return ptree_t::null_vertex();
			}

			auto const& comp = p[i];

			auto const& attribs = tree[n];
			if(comp.name() != attribs.name)
			{
				return ptree_t::null_vertex();
			}

			auto path_leaf = (i == length - 1);
			if(path_leaf)
			{
				return n;
			}

			if(tree.is_leaf(n))
			{
				return ptree_t::null_vertex();
			}

			switch(attribs.type)
			{
				case ParamType::List:
				{
					n = child_by_name(n, p[i + 1].name());
				}
				break;

				case ParamType::Repeat:
				{
					if(!comp.has_index())
					{
						return ptree_t::null_vertex();
					}

					auto out = tree.branches(n);
					n = ptree_t::null_vertex();
					for(auto e : out)
					{
						if(*tree[e].repeat_idx == comp.index())
						{
							n = tree.target(e);
							break;
						}
					}
				}
				break;

				default:
				throw std::runtime_error("malformed tree, terminal param at non-leaf node");
			}
		}
	}

	param_accessor::param_data& param_accessor::node_attribs(node_t n)
	{
		auto& tree = m_pt->tree();
		return tree[n];
	}

	param_accessor::param_data param_accessor::node_attribs(node_t n) const
	{
		auto& tree = m_pt->tree();
		return tree[n];
	}

	param_accessor::Match param_accessor::match_name(std::string const& search, std::string const& node_name)
	{
		auto pos = node_name.find(search);
		if(pos == 0)
		{
			if(search.length() == node_name.length())
			{
				return Match::Exact;
			}
			else
			{
				return Match::Beginning;
			}
		}
		else if(pos != std::string::npos)
		{
			return Match::Anywhere;
		}
		else
		{
			return Match::None;
		}
	}

	bool param_accessor::match_qualifies(Match match, boost::optional< Match > existing, Match method, MatchComparison comp)
	{
		if(match > method)
		{
			return false;
		}

		switch(comp)
		{
			case MatchComparison::Equal:
			return true;

			case MatchComparison::PreferExact:
			return !existing || *existing != Match::Exact || match == Match::Exact;

			case MatchComparison::PreferBeginningOrExact:
			return !existing || match <= *existing;
		}
		throw std::runtime_error{ "invalid matc comparison type" };
	}

	std::set< param_accessor::node_t > param_accessor::find_all_nodes(std::string const& name, indices_t const& indices) const
	{
		if(!has_pt())
		{
			return{};
		}

		std::set< node_t > result;
		boost::optional< Match > current_match_type;

		auto pos = m_location_stk.top();
		auto& tree = m_pt->tree();
		auto nodes = tree.nodes();
		for(auto n : nodes)
		{
			// First, name must match
			auto match = match_name(name, tree[n].name);
			if(!match_qualifies(match, current_match_type, m_match_type, m_comp_type))
			{
				continue;
			}
			current_match_type = current_match_type ? std::min(*current_match_type, match) : match;

			// Now need to check for ambiguity
			auto ancestor = tree.common_ancestor(pos, n);
			auto down_path = tree.path_to_node(ancestor, n).first;
			// Need to see if any node on this path (excluding the last node) is a repeat
			//down_path.pop_back();
			auto end = std::prev(std::end(down_path));
			auto ind_it = std::begin(indices);

			auto ambiguous = false;
			for(auto n_it = std::begin(down_path); n_it != end; ++n_it)
			{
				auto n = *n_it;

				if(tree[n].type == ParamType::Repeat)
				{
					if(ind_it == std::end(indices))
					{
						// No index specified for this repeat, fail
						ambiguous = true;
						break;
					}

					// Check the index value
					auto idx = *ind_it;
					auto next = std::next(n_it);
//					auto edge = tree.in_edge(*next).first;
//					if(*tree[edge].repeat_idx != idx)
					if(tree.position_in_siblings(*next) != idx - 1)
					{
						// Given index does not match on this path, fail
						ambiguous = true;
						break;
					}

					// Matched okay
					++ind_it;
				}
			}

			// Also ensure all given indices were matched
			if(ambiguous || ind_it != std::end(indices))
			{
				continue;
			}

/*			auto repeat = std::find_if(
				std::begin(down_path),
				std::end(down_path),
				[&tree](node_t n)
			{
				return tree[n].type == ParamType::Repeat;
			});

			if(repeat != std::end(down_path))
			{
				// Have to pass downwards through a repeat node, therefore ambiguous search
				continue;
			}
*/
			result.insert(n);
		}

		return result;
	}

	param_accessor::node_t param_accessor::find_node_unambiguous(std::string const& name, indices_t const& indices) const
	{
		auto nodes = find_all_nodes(name, indices);
		return nodes.size() == 1 ? *nodes.begin() : ptree_t::null_vertex();
	}

	param_accessor::node_t param_accessor::node_here() const
	{
		return m_location_stk.top();
	}

	param_accessor::node_t param_accessor::node_at(qualified_path const& path) const
	{
		return node_from_path(path);
	}

}



