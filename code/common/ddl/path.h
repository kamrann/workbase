// path.h

#ifndef __WB_DDL_PATH_H
#define __WB_DDL_PATH_H

#include <boost/variant.hpp>

#include <vector>
#include <string>


namespace ddl {

	class path
	{
	public:
		struct conditional{};

	public:
		path();

	public:
		void append(std::string str);
		void append(unsigned int idx);
		void append(conditional);
		void pop();
		void reset();
		size_t length() const;
		std::string to_string(int idx_base = 1) const;

		static path common_ancestor(path const& p1, path const& p2);

	public:
		bool operator< (path const&) const;
		bool operator== (path const&) const;
		path& operator+= (std::string);
		path& operator+= (unsigned int);
		path& operator+= (conditional);
		//		operator bool() const;
//		bool operator! () const;

	public:
		enum class ComponentType {
			CompositeChild,
			ListItem,
			Conditional,
		};

		class component
		{
		public:
			component();
			component(std::string str);
			component(unsigned int str);
			component(conditional);

			ComponentType type() const;
			std::string as_child() const;
			unsigned int as_index() const;

			std::string to_string(int idx_base = 1) const;

			bool operator< (component const&) const;
			bool operator== (component const&) const;

		private:
			typedef boost::variant< std::string, unsigned int, conditional > comp_var_t;

			comp_var_t var_;
		};

	private:
		typedef std::vector< component > component_list;

		void push_component(component c);

	public:	// ??
		component_list::const_iterator begin() const;
		component_list::const_iterator end() const;

	private:
		component_list components_;

#ifdef _DEBUG
		void dbg_update();

		std::string pathstr_;
#endif
	};

}


#endif


