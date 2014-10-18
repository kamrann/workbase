// reset_cmd_parser.h

#ifndef __WB_SYSSIM_CLIENT_RESET_CMD_PARSER_H
#define __WB_SYSSIM_CLIENT_RESET_CMD_PARSER_H

#include <boost/spirit/include/qi.hpp>
#include <boost/optional.hpp>


namespace qi = boost::spirit::qi;

namespace sys_control {

	struct reset_cmd
	{
		struct seed_data
		{
			struct random_tag{};
			struct same_tag{};

			seed_data(): seed_data{ random_tag{} }	// default behaviour
			{}
			seed_data(unsigned int val): method{ SeedMethod::Fixed }, value{ val }
			{}
			seed_data(random_tag): method{ SeedMethod::Random }
			{}
			seed_data(same_tag): method{ SeedMethod::Same }
			{}

			enum class SeedMethod {
				Fixed,		// Use value member as seed
				Random,		// Seed using clock time
				Same,		// Use same seed as previous run
			};

			SeedMethod method;
			boost::optional< unsigned int > value;
		};

		seed_data seed;
	};

	template < typename Iterator >
	struct reset_cmd_parser: qi::grammar< Iterator, reset_cmd(), qi::space_type >
	{
		reset_cmd_parser();

		qi::rule< Iterator, reset_cmd::seed_data(), qi::space_type > seed;
		qi::rule< Iterator, reset_cmd(), qi::space_type > start;
	};

}


#endif


