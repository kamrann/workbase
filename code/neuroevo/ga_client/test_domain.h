// test_domain.h

#ifndef __WB_GA_CLIENT_TEST_DOMAIN_H
#define __WB_GA_CLIENT_TEST_DOMAIN_H

#include "genetic_algorithm/problem_domain.h"
#include "genetic_algorithm/fixed_binary_genome.h"

#include <string>


class test_domain:
	public ga::i_problem_domain
{
public:
	test_domain(std::string target):
		m_target(target)
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

	virtual ga::mutation mutation_op() const override
	{
		ga::basic_binary_mutation _mutation{};
		return [_mutation](ga::genome& gn, double const rate, ga::rgen_t& rgen) mutable
		{
			_mutation(gn.as< genome_t >(), rate, rgen);
		};
	}

	virtual ga::objective_value_t evaluate_genome(ga::genome const& gn) const override
	{
		auto str = decode(gn.as< genome_t >());

		auto string_length = str.length();
		size_t num_correct = 0;
		for(size_t i = 0; i < string_length; ++i)
		{
			if(str[i] == m_target[i])
			{
				++num_correct;
			}
		}
		return (double)(num_correct) / string_length;
	}

	virtual void output_individual(ga::genome const& gn, std::ostream& os) const
	{
		auto idv = decode(gn.as< genome_t >());
		os << idv;
	}

private:
	static size_t const bits_per_character = 5;

	inline size_t genome_length() const
	{
		auto const string_length = m_target.length();
		return string_length * bits_per_character;
	}

	inline std::string decode(genome_t const& gn) const
	{
		std::string result;
		auto it = gn.cbegin();//std::begin(gn);
		auto string_length = m_target.length();
		for(size_t i = 0; i < string_length; ++i)
		{
			unsigned int ch_idx = 0;
			for(size_t b = 0; b < bits_per_character; ++b)
			{
				if(*it != 0)
				{
					ch_idx |= (1 << b);
				}
				++it;
			}

			char ch = '?';
			if(ch_idx < 26)
			{
				ch = 'a' + ch_idx;
			}
			result += ch;
		}
		return result;
	}

private:
	std::string m_target;
};


#endif


