// cmd_parser.h

#ifndef __WB_PARAMS_CMD_PARSER_H
#define __WB_PARAMS_CMD_PARSER_H

#include "value_parser.h"

#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/adapted/struct/adapt_struct.hpp>

#include <boost/optional.hpp>
#include <boost/variant.hpp>


namespace qi = boost::spirit::qi;

namespace prm {

	struct nav_up_cmd
	{
		boost::optional< std::string > elem;
	};

	struct nav_down_cmd
	{
		std::string elem;
	};
	/*
	typedef boost::variant <
		nav_up_cmd,
		nav_down_cmd
	> nav_cmd;
	*/

	struct nav_cmd
	{
		enum class Direction {
			Up,
			Down,
		};

		nav_cmd()
		{}
		nav_cmd(boost::variant <
			nav_up_cmd,
			nav_down_cmd
		> const& _var)
		{
			if(_var.which() == 0)
			{
				dir = Direction::Up;
				elem = boost::get< nav_up_cmd >(_var).elem;
			}
			else
			{
				dir = Direction::Down;
				elem = boost::get< nav_down_cmd >(_var).elem;
			}
		}

		Direction dir;
		boost::optional< std::string > elem;
	};

	struct depth_type
	{
		depth_type():
			value(1)
		{}
		depth_type(boost::optional< unsigned int > opt):
			value(opt ? *opt : std::numeric_limits< unsigned int >::max())
		{}

		unsigned int value;
	};

	struct list_cmd
	{
		bool required_only;
		bool type_info;
		bool expand_imports;
		boost::optional< nav_cmd > nav;
		boost::optional< depth_type > depth;
	};

	struct setvalue_cmd
	{
		boost::optional< std::string > elem;
		param val;
	};
}

BOOST_FUSION_ADAPT_STRUCT(
	prm::nav_up_cmd,
	(boost::optional< std::string >, elem)
	);

BOOST_FUSION_ADAPT_STRUCT(
	prm::nav_down_cmd,
	(std::string, elem)
	);

BOOST_FUSION_ADAPT_STRUCT(
	prm::list_cmd,
	(bool, required_only)
	(bool, type_info)
	(bool, expand_imports)
	(boost::optional< prm::nav_cmd >, nav)
	(boost::optional< prm::depth_type >, depth)
	);

BOOST_FUSION_ADAPT_STRUCT(
	prm::setvalue_cmd,
	(boost::optional< std::string >, elem)
	(prm::param, val)
	);

namespace prm {

	typedef boost::variant <
		nav_cmd,
		list_cmd,
		setvalue_cmd
	> command;

	template < typename Iterator >
	struct cmd_parser: qi::grammar< Iterator, command(), qi::space_type >
	{
		struct list_symbols: public qi::symbols < char, bool >
		{
			list_symbols()
			{
				add
					("?", false)
					("!", true)
					;
			}
		};

		cmd_parser(): cmd_parser::base_type(start)
		{
			using qi::uint_;
			using qi::lit;
			using qi::alpha;
			using qi::alnum;
			using qi::char_;
			using qi::matches;
			using qi::lexeme;
			using qi::eoi;

			element_name %= lexeme[alpha >> *(alnum | char_('_'))];

			nav_up %= lit('<') >> -element_name;
			nav_down %= lit('>') >> element_name;
			nav %= nav_up | nav_down;

			depth %= lit(':') >> (uint_ | lit("all"));
			list %=
				list_sym >>
				matches[lit('?')] >>
				matches[lit('+')] >>
				-nav >>
				-depth
				;

			setvalue %= lit("set") >> -element_name >> param_value_parser;

			start %=
				(
				nav
				| list
				| setvalue
				)
				>>
				eoi
				;
		}

		qi::rule< Iterator, std::string(), qi::space_type > element_name;
		qi::rule< Iterator, nav_up_cmd(), qi::space_type > nav_up;
		qi::rule< Iterator, nav_down_cmd(), qi::space_type > nav_down;
		qi::rule< Iterator, nav_cmd(), qi::space_type > nav;
		qi::rule< Iterator, depth_type(), qi::space_type > depth;
		qi::rule< Iterator, list_cmd(), qi::space_type > list;
		qi::rule< Iterator, setvalue_cmd(), qi::space_type > setvalue;
		qi::rule< Iterator, command(), qi::space_type > start;

		value_parser< Iterator > param_value_parser;

		list_symbols list_sym;
	};

}


#endif


