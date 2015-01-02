// value_output.cpp

#include "ddl.h"
#include "output.h"


namespace ddl {

	std::ostream& output_value(std::ostream& os, boolean_sch_node::value_t const& val)
	{
		return (os << val << std::endl);
	}

	std::ostream& output_value(std::ostream& os, integer_sch_node::value_t const& val)
	{
		return (os << val << std::endl);
	}

	std::ostream& output_value(std::ostream& os, realnum_sch_node::value_t const& val)
	{
		return (os << val << std::endl);
	}

	std::ostream& output_value(std::ostream& os, string_sch_node::value_t const& val)
	{
		return (os << val << std::endl);
	}

	std::ostream& output_value(std::ostream& os, enum_sch_node::value_t const& val)
	{
		os << "{ ";
		for(auto ev : val)
		{
			os << ev.str << " ";
		}
		os << "}" << std::endl;
		return os;
	}

	std::ostream& output_value(std::ostream& os, std::vector< value_node > const& val)
	{
		for(auto const& c : val)
		{
			os << "- " << c;
		}
		return os;
	}

	std::ostream& output_value(std::ostream& os, std::map< node_name, value_node > const& nd)
	{
		for(auto c : nd)
		{
			os << c.first << ": " << c.second;
		}
		return os;
	}

	struct value_output_visitor:
		public boost::static_visitor< void >
	{
		value_output_visitor(std::ostream& os):
			os_(os)
		{}

		template < typename T >
		inline result_type operator() (T const& val)
		{
			output_value(os_, val);
		}

		std::ostream& os_;
	};

	std::ostream& operator<< (std::ostream& os, value_node const& val)
	{
		auto vis = value_output_visitor{ os };
		val.apply_visitor(vis);
		return os;
	}

}





