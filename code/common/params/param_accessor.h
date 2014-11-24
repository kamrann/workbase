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
		typedef std::vector< size_t > indices_t;

		enum class Match {
			Exact,			// Node name must be identical to search term
			Beginning,		// Node name must begin with (or be identical to) search term
			Anywhere,		// Node name must contain (or be identical to) search term

			None,
		};

		enum class MatchComparison {
			Equal,						// All matches are considered equal
			PreferExact,				// Exact matches hide non-exact matches
			PreferBeginningOrExact,		// Beginning matches hide non-beginning, while exact hide all non-exact
		};

		/*
		Specifies how candidates are located when looking up node by name.
		*/
		enum LookupMode {
			Default,			// Any node is considered, however descendents are preferred to non-descendents, and direct children to indirect descendents
			Descendent,			// Only nodes at or below current position
			Child,				// Only immediate child nodes
		};

		// TODO: Maybe separate above into Lookup { Any, Descendent, Child } and PreferenceFlags { PreferDescendent | PreferDirectChild }

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
		param_data& find_param(qualified_path const& path);
		param_data find_param(qualified_path const& path) const;
		param_data find_param(std::string const& name) const;
		param find_value(qualified_path const& path) const;
		param find_value(std::string const& name) const;
		param_data& param_here();
		param_data param_here() const;
		param& value_here();
		param value_here() const;
		qualified_path find_path(std::string const& name, indices_t const& indices = {}) const;
		qualified_path find_path_descendent(std::string const& name, indices_t const& indices = {}) const;
		qualified_path find_path_ancestor(std::string const& name) const;
		bool move_to(qualified_path const& abs);
		bool move_relative(qualified_path const& rel);
		bool up();
		bool revert();

		void lock();
		void unlock();
		bool is_locked();
		void set_lock_on_failed_move(bool autolock);

		void set_match_method(Match match_type, MatchComparison comp_type);
		void set_lookup_mode(LookupMode mode);

		std::list< qualified_path > children() const;

		operator bool() const;
		bool operator! () const;

		// Shortcuts to create a copy of the accessor with an adjustment
		param_accessor operator() (LookupMode mode) const;
		param_accessor operator() (std::string dest) const;
		param_accessor operator() (qualified_path const& abs) const;
		// TODO: overloaded on Matching modes, also possibly on string/path for relative/absolute move

	private:
		typedef param_tree::tree_t ptree_t;
		typedef ptree_t::node_descriptor node_t;

	public:
		node_t node_here() const;
		node_t node_at(qualified_path const& path) const;

	private:
		bool has_pt() const;
		node_t child_by_name(node_t n, std::string const& name) const;
		qualified_path path_from_node(node_t n) const;
		node_t node_from_path(qualified_path const& p) const;
		param_data& node_attribs(node_t n);
		param_data node_attribs(node_t n) const;

		static Match match_name(std::string const& search, std::string const& node_name);
		static bool match_qualifies(Match match, boost::optional< Match > existing, Match method, MatchComparison comp);
		std::list< std::set< node_t > > get_lookup_candidates() const;
		std::set< node_t > find_all_nodes(std::string const& name, indices_t const& indices = {}) const;
		node_t find_node_unambiguous(std::string const& name, indices_t const& indices = {}) const;

	private:
		param_tree* m_pt;
		std::stack< ptree_t::node_descriptor > m_location_stk;
		bool m_locked;
		bool m_auto_lock;
		Match m_match_type;
		MatchComparison m_comp_type;
		LookupMode m_lookup_mode;
	};

}


#endif


