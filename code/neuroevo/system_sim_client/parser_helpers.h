// parser_helpers.h

#ifndef __WB_PARSER_HELPERS_H
#define __WB_PARSER_HELPERS_H

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/mpl/string.hpp>


namespace qi = boost::spirit::qi;
namespace phx = boost::phoenix;
namespace mpl = boost::mpl;

#if 0

template < int... Name >
struct simple_command_traits
{
	struct command_t{};

	typedef mpl::string< Name... > mpl_str_t;

	typedef std::string::const_iterator iter_t;
	typedef qi::space_type skip_t;
	typedef qi::rule< iter_t, command_t(), skip_t > rule_t;

	static auto parser() -> rule_t
	{
//		static rule_t rule = qi::lit(mpl::c_str< mpl_str_t >::value)[phx::nothing];
//		return rule;
		return qi::lit(mpl::c_str< mpl_str_t >::value)[phx::nothing];
	}
};


template < typename CommandType, typename StatelessParser >
struct spirit_command_traits
{
	typedef CommandType command_t;

	typedef StatelessParser stateless_parser_t;

	typedef std::string::const_iterator iter_t;
	typedef qi::space_type skip_t;
	typedef qi::rule< iter_t, command_t(), skip_t > rule_t;

	static auto parser() -> rule_t
	{
		// TODO: how to avoid needing static grammar?
		// rule.copy() / proto::deep_copy() don't seem to help
		// may be necessary to use a wrapper class which encapsulates a rule, along with
		// container of unique_ptrs to any required grammars
		static stateless_parser_t p;
		return p;
	}
};

template < typename CommandType, typename StatelessParser, int... Prefix >
struct prefixed_grammar_command_traits: spirit_command_traits< CommandType, StatelessParser >
{
	typedef mpl::string< Prefix... > mpl_str_t;

	static auto parser() -> rule_t
	{
		static rule_t rule = spirit_command_traits::parser();
		static rule_t prefixed = qi::lit(mpl::c_str< mpl_str_t >::value) >> rule;
		return prefixed;
	}
};


template < typename... Parsers >
struct rt_alternative_parser;

struct extract_command_type
{
	template < typename _T >
	struct apply
	{
		typedef typename _T::command_t type;
	};
};


// 2 or more, generic case
template < typename First, typename Second, typename... Tail >
struct rt_alternative_parser < First, Second, Tail... >
{
	typedef First this_parser_t;
	typedef rt_alternative_parser< Second, Tail... > tail_t;

	typedef typename mpl::transform <
		mpl::list< First, Second, Tail... >,
		extract_command_type
	> ::type command_types_t;

	typedef typename boost::make_variant_over< command_types_t >::type attr_t;

	typedef std::string::const_iterator iter_t;
	typedef qi::space_type skip_t;
	typedef qi::rule< iter_t, attr_t(), skip_t > rule_t;

	static auto parser() -> rule_t
	{
		// TODO: same as above
		static this_parser_t::rule_t this_r = this_parser_t::parser();
		static tail_t::rule_t tail_r = tail_t::parser();
		static rule_t rule = this_r | tail_r;
		return rule;
	}
};

// Single case
template < typename Single >
struct rt_alternative_parser< Single >
{
	typedef Single this_parser_t;

	typedef typename boost::variant< typename this_parser_t::command_t > attr_t;

	typedef typename this_parser_t::rule_t rule_t;

	static auto parser() -> rule_t
	{
//		static rule_t rule = this_parser_t::parser();
//		return rule;

		return this_parser_t::parser();
	}
};

#endif



#endif


