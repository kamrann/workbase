// real_valued_mutation_ps.h

#ifndef __WB_GA_REAL_VALUED_MUTATION_PS_H
#define __WB_GA_REAL_VALUED_MUTATION_PS_H

#include "../real_valued_mutation.h"

#include "params/param_accessor.h"
#include "params/schema_builder.h"


namespace ga {

	template < typename Rep >
	struct linear_real_gene_mutation_defn
	{
		static void update_schema_provider(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix)
		{
			auto path = prefix;

			(*provider)[path + std::string{ "max_delta" }] = [=](prm::param_accessor acc)
			{
				auto s = sb::real(
					"max_delta",
					0.1,
					0.0
					// TODO: (0-1) parameter normalized by gene range?? 1.0
					);
				return s;
			};

			(*provider)[path] = [=](prm::param_accessor acc)
			{
				auto s = sb::list(path.leaf().name());
				sb::append(s, provider->at(path + std::string{ "max_delta" })(acc));
				return s;
			};
		}

		typedef Rep gene_t;
		typedef ga::gene_mutation< gene_t > result_t;

		static result_t generate(prm::param_accessor acc)
		{
			auto max_delta = prm::extract_as< double >(acc["max_delta"]);
			return ga::real_valued_gene< gene_t >::linear_mutation{ max_delta };
		}
	};

	template < typename Rep >
	struct gaussian_real_gene_mutation_defn
	{
		static void update_schema_provider(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix)
		{
			auto path = prefix;

			(*provider)[path + std::string{ "std_dev" }] = [=](prm::param_accessor acc)
			{
				auto s = sb::real(
					"std_dev",
					1.0,
					0.0
					);
				return s;
			};

			(*provider)[path] = [=](prm::param_accessor acc)
			{
				auto s = sb::list(path.leaf().name());
				sb::append(s, provider->at(path + std::string{ "std_dev" })(acc));
				return s;
			};
		}

		typedef Rep gene_t;
		typedef ga::gene_mutation< gene_t > result_t;

		static result_t generate(prm::param_accessor acc)
		{
			auto std_dev = prm::extract_as< double >(acc["std_dev"]);
			return ga::real_valued_gene< gene_t >::gaussian_mutation{ std_dev };
		}
	};

	template < typename Rep >
	struct variable_gaussian_real_gene_mutation_defn
	{
		static void update_schema_provider(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix)
		{
			auto path = prefix;

			(*provider)[path + std::string{ "initial_stddev" }] = [=](prm::param_accessor acc)
			{
				auto s = sb::real(
					"initial_stddev",
					1.0,	// TODO:
					std::numeric_limits< double >::min()		// stddev cannot be zero
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

			(*provider)[path + std::string{ "cutoff_stddev" }] = [=](prm::param_accessor acc)
			{
				auto s = sb::real(
					"cutoff_stddev",
					1.0,	// TODO:
					std::numeric_limits< double >::min()
					);
				return s;
			};

			(*provider)[path] = [=](prm::param_accessor acc)
			{
				auto s = sb::list(path.leaf().name());
				sb::append(s, provider->at(path + std::string{ "initial_stddev" })(acc));
				sb::append(s, provider->at(path + std::string{ "cutoff_generations" })(acc));
				sb::append(s, provider->at(path + std::string{ "cutoff_stddev" })(acc));
				return s;
			};
		}

		typedef Rep gene_t;
		typedef ga::gene_mutation< gene_t > result_t;

		static result_t generate(prm::param_accessor acc)
		{
			acc.set_lookup_mode(prm::param_accessor::LookupMode::Descendent);
			auto initial = prm::extract_as< double >(acc["initial_stddev"]);
			size_t cutoff_generations = prm::extract_as< int >(acc["cutoff_generations"]);
			auto cutoff = prm::extract_as< double >(acc["cutoff_stddev"]);
			return ga::real_valued_gene< gene_t >::variable_gaussian_mutation{ initial, { cutoff_generations, cutoff } };
		}
	};

}


#endif






