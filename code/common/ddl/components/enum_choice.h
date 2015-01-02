// enum_choice.h

#ifndef __WB_DDL_ENUM_CHOICE_H
#define __WB_DDL_ENUM_CHOICE_H

#include "../schema_defn/specifiers.h"
#include "../schema_defn/defn_node.h"
#include "../values/navigator.h"

#include <string>
#include <functional>


namespace ddl {

	template <
		typename Result, typename... GenArgs
	>
	class enum_choice
	{
	public:
		typedef Result result_t;

		typedef std::function< ddl::defn_node(ddl::specifier&) > defn_fn_t;
		typedef std::function< result_t(navigator, GenArgs...) > generator_fn_t;

	protected:
		struct option
		{
			defn_fn_t defn_fn;
			generator_fn_t gen_fn;
		};

	public:
		enum_choice()
		{}

	public:
		void register_option(std::string name, defn_fn_t defn_fn, generator_fn_t gen_fn)
		{
			options_[name] = { defn_fn, gen_fn };
		}

		template < typename T >
		void register_option(
			std::string name,
			std::shared_ptr< T > ftr
			)
		{
			option opt;
			opt.defn_fn = [ftr](ddl::specifier& spc)
			{
				return ftr->get_defn(spc);
			};
			opt.gen_fn = [ftr](ddl::navigator nav, GenArgs... args) -> result_t
			{
				return ftr->generate(nav, args...);
			};
			options_[name] = opt;
		}

		defn_node get_defn(specifier& spc)
		{
			auto choice_values = define_enum_fixed{};
			for(auto const& opt : options_)
			{
				choice_values = choice_values(opt.first);
			}

			defn_node option = spc.enumeration("option")
				(ddl::define_enum_fixed{ choice_values })
				;
			rf_choice_ = node_ref{ option };

			auto cond_spc = spc.conditional("contents_?");
			for(auto const& opt : options_)
			{
				cond_spc = cond_spc(ddl::spc_condition{
					ddl::cnd::equal{ ddl::node_ref{ option }, opt.first },
					opt.second.defn_fn(spc)
				});
			}
			defn_node cond = cond_spc;

			return spc.composite("objective")(ddl::define_children{}
				("option", option)
				("contents", cond)
				);
		}

		result_t generate(navigator nav, GenArgs... args) const
		{
			auto choice = nav["option"].get().as_single_enum_str();
			return options_.at(choice).gen_fn(nav["contents"][(size_t)0], args...);
		}

		node_ref get_choice_ref()
		{
			return rf_choice_;
		}

	protected:
		std::map< std::string, option > options_;
		node_ref rf_choice_;
	};
		
}


#endif


