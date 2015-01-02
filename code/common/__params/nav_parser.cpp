// nav_parser.cpp

#include "nav_parser.h"

#include <boost/spirit/include/phoenix.hpp>


namespace phx = boost::phoenix;

namespace prm {

	template < typename Iterator >
	nav_parser< Iterator >::nav_parser(bool restricted, bool enable_indexed):
		nav_parser::base_type(start)
	{
		using qi::uint_;
		using qi::lit;
		using qi::alpha;
		using qi::alnum;
		using qi::char_;
		using qi::lexeme;
		using qi::_val;
		using qi::_1;
		using qi::_2;

		nav_search_term =
			lexeme[alpha >> *(alnum | char_('_'))];

		rpt_indices_list =
			*(lit('/') >> uint_)
			;

		nav_path =
			(nav_search_term >> rpt_indices_list)
			[
				_val = phx::construct< nav_component >(
				phx::val(undirected_nav_tag{}), _1, _2
				)
			]
		;

		nav_up =
			lit('<') >>
			(-nav_search_term)
			[
				_val = phx::construct< nav_component >(phx::val(nav_up_tag{}), _1)
			]
		;

		nav_down =
			lit('>') >>
			(nav_search_term >> rpt_indices_list)
			[
				_val = phx::construct< nav_component >(
				phx::val(nav_down_tag{}), _1, _2
				)
			]
		;

		nav_index =
			uint_
			[
				_val = phx::construct< nav_component >(phx::val(nav_by_index_tag{}), _1)
			];

		start = nav_up | nav_down;
		if(enable_indexed)
		{
			start = start.copy() | nav_index;
		}
		if(!restricted)
		{
			start = start.copy() | nav_path;
		}
	}

	template struct nav_parser < std::string::const_iterator > ;

}


