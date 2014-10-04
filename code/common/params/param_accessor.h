// param_accessor.h

#ifndef __WB_PARAM_ACCESSOR_H
#define __WB_PARAM_ACCESSOR_H

#include "param_tree.h"
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
		typedef param_tree::param_data param_data;

	public:
		param_accessor(param_tree* pt = nullptr);

	public:
		qualified_path where() const;
		bool at_leaf() const;
		bool exists(qualified_path const& path) const;
		bool is_available(std::string const& name) const;
		param& operator[] (std::string const& name);
		param operator[] (std::string const& name) const;
		param& operator[] (char const* const name);
		param operator[] (char const* const name) const;
		param_data find_param(qualified_path const& path) const;
		param_data find_param(std::string const& name) const;
		param find_value(qualified_path const& path) const;
		param find_value(std::string const& name) const;
		param_data param_here() const;
		param& value_here();
		param value_here() const;
		qualified_path find_path(std::string const& name) const;
		qualified_path find_path_descendent(std::string const& name) const;
		qualified_path find_path_ancestor(std::string const& name) const;
		//param_accessor from_absolute(qualified_path const& path) const;
		bool move_to(qualified_path const& abs);
		bool move_relative(qualified_path const& rel);
		bool up();
		bool revert();

		std::list< qualified_path > children() const;

		operator bool() const;
		bool operator! () const;

	private:
		typedef param_tree::tree_t ptree_t;
		typedef ptree_t::node_descriptor node_t;
//		typedef ptree_t::node_attribs_t node_attribs_t;

	private:
		bool has_pt() const;
		node_t child_by_name(node_t n, std::string const& name) const;
		qualified_path path_from_node(node_t n) const;
		node_t node_from_path(qualified_path const& p) const;
		//node_attribs_t&
		param_data& node_attribs(node_t n);
		param_data node_attribs(node_t n) const;

		bool match_name(std::string const& search, std::string const& node_name) const;
		std::set< node_t > find_all_nodes(std::string const& name) const;
		node_t find_node_unambiguous(std::string const& name) const;

	private:
		param_tree* m_pt;
		std::stack< ptree_t::node_descriptor > m_location_stk;
	};



#if 0
	class param_accessor
	{
	public:
		

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
#endif
}


#endif


