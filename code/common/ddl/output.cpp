// output.cpp

#include "ddl.h"
#include "output.h"


namespace ddl {

	std::ostream& operator<< (std::ostream& os, boolean_sch_node const& nd)
	{
		os << "boolean" << " (";
		os << "dft=" << nd.default();
		os << ")" << std::endl;
		return os;
	}

	std::ostream& operator<< (std::ostream& os, integer_sch_node const& nd)
	{
		return (os << "integer" << std::endl);
	}

	std::ostream& operator<< (std::ostream& os, realnum_sch_node const& nd)
	{
		return (os << "realnum" << std::endl);
	}

	std::ostream& operator<< (std::ostream& os, string_sch_node const& nd)
	{
		return (os << "string" << std::endl);
	}

	std::ostream& operator<< (std::ostream& os, enum_sch_node const& nd)
	{
		return (os << "enumeration" << std::endl);
	}

	std::ostream& operator<< (std::ostream& os, list_sch_node const& nd)
	{
		return (os << "list" << std::endl);
	}

	std::ostream& operator<< (std::ostream& os, composite_sch_node const& nd)
	{
		os << "composite" << std::endl;
		auto children = nd.children();
		os << "{" << std::endl;
		for(auto c : children)
		{
			os << c.first << ": " << c.second;
		}
		os << "}" << std::endl;
		return os;
	}

	struct output_visitor:
		public boost::static_visitor< void >
	{
		output_visitor(std::ostream& os):
			os_(os)
		{}

		template < typename Node >
		inline result_type operator() (Node const& nd)
		{
			os_ << nd;
		}

		std::ostream& os_;
	};

	std::ostream& operator<< (std::ostream& os, sch_node const& schema)
	{
		auto vis = output_visitor{ os };
		schema.apply_visitor(vis);
		return os;
	}

}





