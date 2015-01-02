// param_accessor.h

#ifndef __WB_PARAM_ACCESSOR_H
#define __WB_PARAM_ACCESSOR_H

#include "param.h"
#include "qualified_path.h"
#include "unspecified.h"	// For convenience of including file

#include <yaml-cpp/yaml.h>

#include <boost/optional.hpp>

#include <string>
#include <stack>
#include <set>


namespace prm
{
	class param_accessor
	{
	public:
		param operator[] (std::string const& name) const
		{
			// TODO: new impl not taking into account m_ignore
			return find_value(name);
		}

		inline param find_node(qualified_path const& path) const
		{
			return prm::find_node(m_root, path);
		}

		inline param find_node(std::string const& name) const
		{
			auto path = find_path(name);
			return find_node(path);
		}

		inline param find_value(qualified_path const& path) const
		{
			auto node = find_node(path);
			return node[ParamNode::Value];
		}

		inline param find_value(std::string const& name) const
		{
			auto node = find_node(name);
			return node[ParamNode::Value];
		}

		param node_here() const
		{
			return find_node(where());
		}

		param value_here() const
		{
			return find_value(where());
		}

		qualified_path find_path(std::string const& name) const
		{
			if(!m_root.IsNull())
			{
				auto resolved = resolve_id(name, m_root, get_current_path(), m_all_paths);
				//if(resolved != qualified_path() && (
				//	!m_ignore || !resolved.is_same_or_descendent_of(*m_ignore)))
				return resolved;
			}
			else
			{
				return qualified_path();
			}
		}

		qualified_path find_path_descendent(std::string const& name) const
		{
			// TODO: need to be able to deal with case where name matches both an ancestor and an unambiguous 
			// descendent. currently this will fail if the ancestor comes first in the list of all paths
			auto path = find_path(name);
			return path.is_same_or_descendent_of(where()) ? path : qualified_path{};
		}

		qualified_path find_path_ancestor(std::string const& name) const
		{
			// TODO: need to be able to deal with case where name matches both an ancestor and an unambiguous 
			// descendent. currently this will fail if the descendent comes first in the list of all paths
			auto path = find_path(name);
			return where().is_same_or_descendent_of(path) ? path : qualified_path{};
		}

		param_accessor from_absolute(qualified_path const& path) const
		{
			param_accessor copy(*this);
			copy.m_path_stk = std::stack< qualified_path >();
			copy.m_path_stk.push(path);
			return copy;
		}

		void push_relative_path(qualified_path const& rel)
		{
			m_path_stk.push(get_current_path() + rel);
		}

		void pop_relative_path()
		{
			m_path_stk.pop();
		}

		inline void up()
		{
			auto parent = where();
			parent.pop();
			m_path_stk.push(parent);
		}

	public:
		param_accessor(
			param p = param(),
			/*param_tree* tree = nullptr,*/
			qualified_path const& path = qualified_path(),
			boost::optional< qualified_path > const& ignore = boost::none
			):
			m_root(p),
			m_ignore(ignore)
		{
			m_path_stk.push(path);
			m_all_paths = construct_abs_paths(m_root, qualified_path());
		}

		param get_root() const
		{
			return m_root;
		}

	public:
		inline qualified_path get_current_path() const
		{
			return m_path_stk.top();
		}

		inline qualified_path where() const
		{
			return m_path_stk.top();
		}

		inline bool at_leaf() const
		{
			return m_path_stk.top().is_leaf();
		}

		inline std::set< qualified_path > const& get_all_paths() const
		{
			return m_all_paths;
		}

	private:
		param m_root;
		boost::optional< qualified_path > m_ignore;
		std::stack< qualified_path > m_path_stk;

		std::set< qualified_path > m_all_paths;
	};
}


#endif


