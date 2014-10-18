// nav_parser.h

#ifndef __WB_PARAMS_NAV_PARSER_H
#define __WB_PARAMS_NAV_PARSER_H

#include <boost/spirit/include/qi.hpp>

#include <boost/optional.hpp>


namespace qi = boost::spirit::qi;

namespace prm {

	struct nav_up_tag{};
	struct nav_down_tag{};
	struct nav_by_index_tag{};
	struct undirected_nav_tag{};

	struct nav_component
	{
		boost::optional< unsigned int > nav_index;

		enum class Direction {
			Up,
			Down,
		};

		boost::optional< Direction > dir;

		struct search_element_t
		{
			std::string term;
			std::vector< unsigned int > rpt_indices;
		};

		boost::optional< search_element_t > search_element;


		nav_component()
		{}

		nav_component(nav_by_index_tag, unsigned int _idx):
			nav_index{ _idx}
		{}

		nav_component(nav_up_tag, boost::optional< std::string > _term):
			dir{ Direction::Up }
		{
			if(_term)
			{
				search_element = search_element_t{ *_term };
			}
		}

		nav_component(nav_down_tag, std::string _term, std::vector< unsigned int > const& _rpt_indices):
			dir{ Direction::Down },
			search_element{ search_element_t{ _term, _rpt_indices } }
		{}

		nav_component(undirected_nav_tag, std::string _term, std::vector< unsigned int > const& _rpt_indices):
			search_element{ search_element_t{ _term, _rpt_indices } }
		{}
	};


	template < typename Iterator >
	struct nav_parser: qi::grammar< Iterator, nav_component(), qi::space_type >
	{
		nav_parser(bool restricted, bool enable_indexed);

		qi::rule< Iterator, std::string(), qi::space_type > nav_search_term;
		qi::rule< Iterator, std::vector< unsigned int >(), qi::space_type > rpt_indices_list;
		qi::rule< Iterator, nav_component(), qi::space_type > nav_path;
		qi::rule< Iterator, nav_component(), qi::space_type > nav_up;
		qi::rule< Iterator, nav_component(), qi::space_type > nav_down;
		qi::rule< Iterator, nav_component(), qi::space_type > nav_index;
		qi::rule< Iterator, nav_component(), qi::space_type > start;
	};

}


#endif


