// param_accessor.h

#ifndef __WB_PARAM_ACCESSOR_H
#define __WB_PARAM_ACCESSOR_H

#include "params/param.h"
#include "qualified_path.h"
#include "pw_unspecified.h"	// For convenience of including file

#include <yaml-cpp/yaml.h>

#include <boost/optional.hpp>

#include <stack>


namespace prm
{
	class param_accessor
	{
	public:
		param operator[] (std::string const& name) const
		{
			if(!m_root.IsNull())
			{
				auto resolved = resolve_id(name, m_root, get_current_path(), m_all_paths);
				if(resolved != qualified_path() && (
					!m_ignore || !resolved.is_same_or_descendent_of(*m_ignore)))
				{
					return find_value(m_root, resolved);
				}
			}
			return param(YAML::NodeType::Undefined);
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


