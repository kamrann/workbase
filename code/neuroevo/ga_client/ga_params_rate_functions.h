// ga_params_rate_functions.h

#ifndef __WB_GA_PARAMS_RATE_FNS_H
#define __WB_GA_PARAMS_RATE_FNS_H

#include "genetic_algorithm/rate_function.h"
#include "genetic_algorithm/fixed_rate_function.h"
#include "genetic_algorithm/generation_variable_rate_function.h"

#include "params/param_accessor.h"
#include "params/schema_builder.h"


namespace ga {

	namespace sb = prm::schema;

	struct fixed_rate_fn_defn
	{
/*		TODO: for this will need to make members non-static
		fixed_rate_fn_defn(double default_rate):
			_default(default_rate)
		{}
*/
		static void update_schema_provider(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix)
		{
			auto path = prefix;

			(*provider)[path + std::string{ "rate" }] = [=](prm::param_accessor acc)
			{
				auto s = sb::real(
					"rate",
					1.0,	// TODO:
					0.0,
					1.0
					);
				return s;
			};

			(*provider)[path] = [=](prm::param_accessor acc)
			{
				auto s = sb::list(path.leaf().name());
				sb::append(s, provider->at(path + std::string{ "rate" })(acc));
				return s;
			};
		}

		typedef ga::rate_fn_t result_t;

		static result_t generate(prm::param_accessor acc)
		{
			auto rate = prm::extract_as< double >(acc["rate"]);
			return ga::fixed_rate_fn{ rate };
		}

		double _default;
	};

	struct generation_variable_rate_fn_defn
	{
		/*		TODO: for this will need to make members non-static
		fixed_rate_fn_defn(double default_rate):
		_default(default_rate)
		{}
		*/
		static void update_schema_provider(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix)
		{
			auto path = prefix;

			(*provider)[path + std::string{ "initial" }] = [=](prm::param_accessor acc)
			{
				auto s = sb::real(
					"initial",
					1.0,	// TODO:
					0.0,
					1.0
					);
				return s;
			};

			(*provider)[path + std::string{ "cutoff_generations" }] = [=](prm::param_accessor acc)
			{
				auto s = sb::integer(
					"cutoff_generations",
					100,	// TODO:
					1
					);
				return s;
			};

			(*provider)[path + std::string{ "cutoff_rate" }] = [=](prm::param_accessor acc)
			{
				auto s = sb::real(
					"cutoff_rate",
					0.0,	// TODO:
					0.0,
					1.0
					);
				return s;
			};

			(*provider)[path] = [=](prm::param_accessor acc)
			{
				auto s = sb::list(path.leaf().name());
				sb::append(s, provider->at(path + std::string{ "initial" })(acc));
				sb::append(s, provider->at(path + std::string{ "cutoff_generations" })(acc));
				sb::append(s, provider->at(path + std::string{ "cutoff_rate" })(acc));
				return s;
			};
		}

		typedef ga::rate_fn_t result_t;

		static result_t generate(prm::param_accessor acc)
		{
			acc.set_lookup_mode(prm::param_accessor::LookupMode::Descendent);
			auto initial = prm::extract_as< double >(acc["initial"]);
			auto cutoff_generations = prm::extract_as< size_t >(acc["cutoff_generations"]);
			auto cutoff_rate = prm::extract_as< double >(acc["cutoff_rate"]);
			return ga::generation_variable_rate_fn{
				initial,
				ga::generation_variable_rate_fn::control_point{ cutoff_generations, cutoff_rate }
			};
		}

		double _default;
	};	

}


#endif


