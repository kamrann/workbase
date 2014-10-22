// clsm.h
/*
Command Line State Machine
*/

#ifndef __WB_COMMAND_LINE_STATE_MACHINE_H
#define __WB_COMMAND_LINE_STATE_MACHINE_H

#include "choice_point.h"
#include "parser_helpers.h"

#include <boost/statechart/event.hpp>
#include <boost/statechart/state.hpp>
#include <boost/statechart/custom_reaction.hpp>
#include <boost/variant/static_visitor.hpp>
#include <boost/mpl/list.hpp>

#include <string>


namespace clsm {

	namespace sc = boost::statechart;
	namespace mpl = boost::mpl;


	struct ev_command:
		sc::event < ev_command >
	{
		std::string cmd_str;

		ev_command(std::string _cmd):
			cmd_str{ _cmd }
		{}
	};

	typedef std::string::const_iterator iter_t;
	typedef qi::space_type skip_t;

	/*
	template < typename Dispatcher >
	struct cmd_visitor:
		public boost::static_visitor < sc::result >
	{
		cmd_visitor(Dispatcher& disp): dispatcher{ disp }
		{}

		template < typename T >
		inline result_type operator() (T const& val)
		{
			return dispatcher.dispatch(val);
		}

		Dispatcher& dispatcher;
	};
	*/

	struct i_cmd_state
	{
		virtual std::string get_prompt() const = 0;
	};


	template < typename MostDerived, typename Context, typename InnerInitial = mpl::list<> >
		//, typename... CommandTraits >
	struct clsm_state_base: public sc::state < MostDerived, Context, InnerInitial >
	{
//		using state::state;
		clsm_state_base(my_context ctx): state(ctx)
		{}

		typedef clsm_state_base clsm_state_base_t;

		typedef mpl::list<
			sc::custom_reaction< ev_command >
		> reactions;

		// Signature for a function which processes an (already parsed) command, and returns the
		// reaction result.
		typedef std::function< sc::result() > command_reaction_fn;

		// Signature for a registered parser/dispatcher function.
		typedef std::function< command_reaction_fn(std::string const&) > try_parse_fn;

		// Reaction to receiving command
		sc::result react(ev_command const& ev)
		{
			// Try to parse command using each registered parser in turn
			for(auto const& fn : try_parse_list)
			{
				auto res = fn(ev.cmd_str);
				if(res)
				{
					// Parse was successful, dispatch and return reaction result
					return res();
				}
			}

			// No registered parser matched, pass on to outer state
			return forward_event();
		}

		// A parser_provider is a nullary function returning a rule with an associated attribute (parsed command) type
		template < typename Command >
		using parser_provider = std::function < qi::rule< iter_t, Command(), skip_t >() > ;

		// Helpers for wrapping existing grammars and rules
		template < typename Grammar, typename... Args >
		auto wrap_grammar(Args... args) -> parser_provider < typename typename Grammar::start_type::attr_type >
		{
			typedef typename typename Grammar::start_type::attr_type command_t;

			auto ptr = std::make_shared< Grammar >(args...);
			qi::rule< iter_t, command_t(), skip_t > rule = *ptr;
			return[ptr, rule]{ return rule; };
		}

		template < typename Grammar, typename... Args >
		auto wrap_grammar_prefixed(std::string prefix, Args... args) -> parser_provider < typename typename Grammar::start_type::attr_type >
		{
			typedef typename typename Grammar::start_type::attr_type command_t;

			auto ptr = std::make_shared< Grammar >(args...);
			qi::rule< iter_t, command_t(), skip_t > rule = qi::lit(prefix) >> *ptr;
			return[ptr, rule]{ return rule; };
		}

		template < typename Command >
		auto wrap_rule_temp(qi::rule< iter_t, Command, skip_t > rule) -> parser_provider < Command >
		{
			return[=]{ return rule; };
		}

		// Registers a parser_provider with the state.
		template < typename Command >
		void reg_cmd(parser_provider< Command > pp)
		{
			// The try_parse function invokes the parser with the command input string.
			// If successful, it returns a command reaction function which will, when called,
			// will process the command and return the reaction result.
			try_parse_fn fn = [=](std::string const& cmd_str) -> command_reaction_fn
			{
				auto rule = pp();
				auto parser = rule >> qi::eoi;
				Command cmd;
				auto it = std::begin(cmd_str);
				auto result = qi::phrase_parse(it, std::end(cmd_str), parser, skip_t{}, cmd);

				/*
				TODO:
				Ideally at this stage, an attempted parse could result in:
				1- Success.
				2- Fail, try other command parsers
				3- Fail acting like spirit expectation point- don't try further parsers, output parse failure info.

				Really this should be incorporated into the spirit parsers as expectation points.
				*/

				if(result)
				{
					// Successful parse. Reaction function is dispatch method for associated command type in
					// the derived state class.
					return [=]{ return static_cast<MostDerived*>(this)->dispatch(cmd); };
				}
				else
				{
					// Failed to match this parser, return uninitialized function signifying that subsequent
					// parsers should be attempted.
					return{};
				}
			};

			try_parse_list.push_back(fn);
		}

		std::list< try_parse_fn > try_parse_list;
	};

#if 0
	/* to do template way, must store not simply a ct type list, but a rt tuple<...> of trait objects, which are constructed by the
	owning fsm state (in its constructor) and provide an actual parser provider, as well as the parsed attr type.
	*/

	typedef rt_alternative_parser< CommandTraits... > cmd_parser_gen_t;
	typedef typename cmd_parser_gen_t::rule_t cmd_parser_t;

	sc::result process_command(std::string const& cmd_str)
	{
		cmd_parser_t alt_parser = cmd_parser_gen_t::parser();
		cmd_parser_t parser = alt_parser >> qi::eoi;

		typedef typename cmd_parser_gen_t::attr_t parsed_command_t;
		parsed_command_t cmd;

		auto it = std::begin(cmd_str);
		auto result = qi::phrase_parse(it, std::end(cmd_str), parser, skip_t{}, cmd);

		if(!result)
		{
			return forward_event();
		}

		clsm::cmd_visitor< MostDerived > vis{ *static_cast< MostDerived* >(this) };
		return boost::apply_visitor(vis, cmd);
	}
#endif

	template < typename MostDerived, typename Context >
	struct innermost_state_base: public sc::state < MostDerived, Context >
	{
		typedef innermost_state_base innermost_base_t;

		struct ev_update_prompt: sc::event < ev_update_prompt > {};

		/*			typedef mpl::list<
						sc::custom_reaction< ev_update_prompt >
						> reactions;
						*/
		innermost_state_base(my_context ctx): my_base(ctx)
		{
			//				post_event(ev_update_prompt{});
		}

		/*			sc::result react(ev_update_prompt const&)
					{
					auto prompt = state_cast<i_cmd_state const&>().get_prompt();
					context< paramtree_editor >().prompt_callback(prompt);
					return discard_event();
					}
					*/
	};

}


#endif


