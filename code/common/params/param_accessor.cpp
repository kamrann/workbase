// param_accessor.cpp

#include "param_accessor.h"
#include "param_yaml.h"


namespace prm
{
	param_accessor::param_accessor(param_tree* pt):
		m_pt{ pt }
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

	qualified_path param_accessor::find_path(std::string const& name) const
	{
		auto n = find_node_unambiguous(name);
		return n == ptree_t::null_vertex() ? qualified_path{} : path_from_node(n);
	}

	qualified_path param_accessor::find_path_descendent(std::string const& name) const
	{
		// TODO: need to be able to deal with case where name matches both an ancestor and an unambiguous 
		// descendent. currently this will fail if the ancestor comes first in the list of all paths
		auto nodes = find_all_nodes(name);
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

//		auto path = find_path(name);
//		return path.is_same_or_descendent_of(where()) ? path : qualified_path{};
	}

	qualified_path param_accessor::find_path_ancestor(std::string const& name) const
	{
		// TODO: need to be able to deal with case where name matches both an ancestor and an unambiguous 
		// descendent. currently this will fail if the descendent comes first in the list of all paths
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

//		auto path = find_path(name);
//		return where().is_same_or_descendent_of(path) ? path : qualified_path{};
	}

	bool param_accessor::move_to(qualified_path const& abs)
	{
		auto n = node_from_path(abs);
		if(n == ptree_t::null_vertex())
		{
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
		return move_to(where() + rel);
	}

	bool param_accessor::up()
	{
		auto parent = where();
		parent.pop();
		return move_to(parent);
	}

	bool param_accessor::revert()
	{
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
		qualified_path qpath;
		auto& tree = m_pt->tree();
		ptree_t::path tree_path = tree.path_to_node(node);
		for(auto n : tree_path)
		{
			auto attribs = tree[n];
			qpath += attribs.name;

			if(attribs.type == ParamType::Repeat)
			{
				auto e = tree.path_edge_from_node(tree_path, n).first;
				qpath.leaf().set_index(*tree[e].repeat_idx);
			}
		}
		return qpath;
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

	bool param_accessor::match_name(std::string const& search, std::string const& node_name) const
	{
		if(false)	// todo: exact
		{
			return node_name == search;
		}
		else // assuming left match
		{
			return node_name.find(search) == 0;
		}
	}

	std::set< param_accessor::node_t > param_accessor::find_all_nodes(std::string const& name) const
	{
		if(!has_pt())
		{
			return{};
		}

		std::set< node_t > result;
		auto pos = m_location_stk.top();
		auto& tree = m_pt->tree();
		auto nodes = tree.nodes();
		for(auto n : nodes)
		{
			// First, name must match
			if(!match_name(name, tree[n].name))
			{
				continue;
			}

			// Now need to check for ambiguity
			auto ancestor = tree.common_ancestor(pos, n);
			auto down_path = tree.path_to_node(ancestor, n).first;
			// Need to see if any node on this path (excluding the last node) is a repeat
			down_path.pop_back();
			auto repeat = std::find_if(
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

			result.insert(n);
		}

		return result;
	}

	param_accessor::node_t param_accessor::find_node_unambiguous(std::string const& name) const
	{
		auto nodes = find_all_nodes(name);
		return nodes.size() == 1 ? *nodes.begin() : ptree_t::null_vertex();
	}

}



