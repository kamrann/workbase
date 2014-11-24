// function_min_max_domain.h

#ifndef __WB_GA_FUNCTION_MIN_MAX_DOMAIN_H
#define __WB_GA_FUNCTION_MIN_MAX_DOMAIN_H

#include "genetic_algorithm/problem_domain.h"
#include "genetic_algorithm/fixed_binary_genome.h"

#include <string>


class function_min_max_domain:
	public ga::i_problem_domain
{
public:
	typedef int value_t;

	struct parameter_defn
	{
		value_t lower_bound;
		value_t upper_bound;

		parameter_defn():
			lower_bound(std::numeric_limits< value_t >::min()),
			upper_bound(std::numeric_limits< value_t >::max())
		{}

		inline unsigned long long range() const
		{
			return (unsigned long long)upper_bound - lower_bound;
		}

		inline int required_bits() const
		{
			auto max_rep = range();
			int bits = 0;
			while(max_rep != 0)
			{
				++bits;
				max_rep >>= 1;
			}
			return bits;
		}
	};

	typedef std::vector< value_t > parameter_list;

	struct function_defn
	{
		std::vector< parameter_defn > parameters;

		std::function< value_t(parameter_list const& params) > eval;
	};

	enum class Goal {
		Minimise,
		Maximise,
	};

public:
	function_min_max_domain(function_defn defn, Goal goal):
		m_defn(defn),
		m_goal(goal)
	{}

	typedef ga::fixed_binary_genome genome_t;

public:
	virtual ga::genome create_random_genome(ga::rgen_t& rgen) const override
	{
		return std::make_unique< genome_t >(genome_t::generate_random(genome_length(), rgen));
	}

	virtual ga::crossover_fn_t crossover_op() const override
	{
		ga::basic_array_crossover< genome_t > _crossover{ genome_length() };
		return [_crossover](ga::genome const& p1, ga::genome const& p2, ga::rgen_t& rgen) mutable
		{
			return std::make_unique< genome_t >(_crossover(p1.as< genome_t >(), p2.as< genome_t >(), rgen));
		};
	}

	virtual ga::mutation_fn_t mutation_op() const override
	{
		ga::basic_binary_mutation _mutation{};
		return [_mutation](ga::genome& gn, double const rate, ga::rgen_t& rgen) mutable
		{
			_mutation(gn.as< genome_t >(), rate, rgen);
		};
	}

	virtual ga::objective_value_t evaluate_genome(ga::genome const& gn) const override
	{
		auto params = decode(gn.as< genome_t >());
		auto output = m_defn.eval(params);
		return m_goal == Goal::Maximise ? output : -output;
	}

	virtual void output_individual(ga::genome const& gn, std::ostream& os) const
	{
		auto idv = decode(gn.as< genome_t >());
		os << "[ ";
		for(auto const& v : idv)
		{
			os << v << " ";
		}
		os << "]";
	}

private:
	inline size_t genome_length() const
	{
		size_t len = 0;
		for(auto const& pd : m_defn.parameters)
		{
			auto required_bits = pd.required_bits();
			len += required_bits;
		}
		return len;
	}

	inline parameter_list decode(genome_t const& gn) const
	{
		parameter_list result;
		auto it = gn.cbegin();
		for(auto const& pd : m_defn.parameters)
		{
			auto required_bits = pd.required_bits();
			unsigned long long offset = 0;
			for(size_t b = 0; b < required_bits; ++b, ++it)
			{
				if(*it != 0)
				{
					offset |= (1 << b);
				}
			}
			value_t val = pd.lower_bound + (long long)offset;
			result.push_back(val);
		}
		return result;
	}

private:
	function_defn m_defn;
	Goal m_goal;
};


namespace predefined {

	function_min_max_domain::function_defn ackley(
		size_t n_dimensions,
		double lower = -40.0,
		double upper = 40.0,
		double a = 20.0,
		double b = 0.2,
		double c = 2 * M_PI
		)
	{
		function_min_max_domain::function_defn defn;
		defn.parameters.resize(n_dimensions);
		for(auto& pd : defn.parameters)
		{
			pd.lower_bound = lower;
			pd.upper_bound = upper;
		}

		defn.eval = [n_dimensions, a, b, c](function_min_max_domain::parameter_list const& p)
		{
			double sum_x_2 = 0.0;
			double sum_cos_cx = 0.0;
			for(auto const& x : p)
			{
				sum_x_2 += x * x;
				sum_cos_cx += std::cos(c * x);
			}

			return -a * std::exp(-b * std::sqrt(sum_x_2 / n_dimensions))
				- std::exp(sum_cos_cx / n_dimensions)
				+ a
				+ M_E;
		};

		return defn;
	}

}



#endif


