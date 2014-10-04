// qualified_path.h

#ifndef __WB_QUALIFIED_PATH_H
#define __WB_QUALIFIED_PATH_H

#include <boost/optional.hpp>

#include <string>
#include <vector>
#include <tuple>
#include <ostream>


namespace prm {

	class qualified_path
	{
	public:
		static std::string const ScopeString;
		static std::string const IndexPrefix;
		static std::string const IndexSuffix;

		class component
		{
		public:
			component(std::string const& _name = std::string(), boost::optional< size_t > const& _index = boost::none);

		public:
			std::string const& name() const;
			bool has_index() const;
			size_t index() const;
			std::string to_string(std::string scope_str = ScopeString) const;
			void set_index(size_t idx);

		public:
			static bool same_name(component const& lhs, component const& rhs);
			static bool same_index(component const& lhs, component const& rhs);

		public:
			bool operator== (component const& rhs) const;
			bool operator< (component const& rhs) const;

		private:
			std::string m_name;
			boost::optional< size_t > m_index;
		};

		typedef std::vector< component > component_list_t;
		typedef component_list_t::const_iterator const_iterator;

	public:
		qualified_path();
		qualified_path(std::string const& path_str);

	public:
		std::string to_string(std::string scope_str = ScopeString, bool include_base_scope = true) const;
		static qualified_path from_string(std::string const& path_str);
		size_t size() const;
		bool is_empty() const;
		bool is_leaf() const;
		bool contains_indexes() const;
		component& root();
		component& leaf();
		component const& root() const;
		component const& leaf() const;
		const_iterator begin() const;
		const_iterator end() const;
		// Returns the path resulting from stripping off the leaf
		qualified_path stem() const;
		// Returns the path resulting from stripping off the base (first section)
		qualified_path tail() const;
		qualified_path subpath(const_iterator from) const;
		qualified_path subpath(const_iterator from, const_iterator to) const;
		qualified_path unindexed() const;
		// This tests if a relative path matches the full path (ignoring indexes), and if so,
		// returns an iterator to the corresponding component.
		// TODO: Make a relative_path class? Or modify this one to represent both partially and fully qualified paths?
		// Currently just assuming unqualified is a leaf only
		const_iterator find(std::string const& unqualified) const;
		// Looks for single component matching name given
		const_iterator find_anywhere(std::string const& unqualified) const;
		// Finds returns true only if p is contained at the beginning of the range represented by this (ie. share same root)
		bool is_same_or_descendent_of(qualified_path const& p) const;
		// Determines whether or not the current path is unambiguous from source, in the sense that it has no
		// indexes after the point of divergence.
		bool is_unambiguous_from(qualified_path const& source) const;

	public:
		/*
		Returns iterators designating where the two paths diverge, if at all.
		{ end, end, false, false } -> identical paths
		{ end, x, false, false } -> rhs contains lhs
		{ x, end, false, false } -> lhs contains rhs
		{ x, x, false, false } -> diverge by differing component names
		{ x, x, true, true } -> diverge by differing indexes
		{ x, x, true, false } -> diverge by lhs having index, rhs having no index, at component with same name
		{ x, x, false, true } -> diverge by lhs having no index, rhs having index, at component with same name
		*/
		static std::tuple<
			const_iterator,	// lhs iterator
			const_iterator,	// rhs iterator
			bool,			// lhs index
			bool			// rhs index
		>
		divergence(qualified_path const& lhs, qualified_path const& rhs);

		static std::tuple<
			bool,
			bool
		>
		indexed_after_divergence(qualified_path const& lhs, qualified_path const& rhs);

		//static bool match_indexes(qualified_path const& lhs, qualified_path const& rhs);

	public:
		qualified_path& operator+= (qualified_path const& rhs);
		qualified_path operator+ (qualified_path const& rhs) const;
		void pop();
		bool operator== (qualified_path const& rhs) const;
		bool operator!= (qualified_path const& rhs) const;
		bool operator< (qualified_path const& rhs) const;
		component const& operator[] (int idx) const;
		operator bool() const;
		bool operator! () const;

	private:
		component_list_t m_components;
	};

	std::ostream& operator<< (std::ostream& os, qualified_path const& qp);

}



#endif


