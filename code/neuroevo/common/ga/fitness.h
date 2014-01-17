// fitness.h

#ifndef __FITNESS_H
#define __FITNESS_H

#include <boost/range/iterator_range.hpp>

#include <string>
#include <sstream>

/*
Objective value - result of an objective (achievement) function.
	must be comparable as minimum.

Composite objective (actually method to generate real obj val from multiple real obj vals) -
	combine multiple objective values using weighted sum, product, quotient, etc.
	all objective values must be real valued, as will be the composite result.

Pareto objective (Note aside, this is in fact a method to generate an integer obj val from multiple comparable obj values) -
	combine multiple objective values by pareto comparison, then pareto rank them (so obj val type winds up integer)
NOTE: Think could potentially come up with a rough tournament selection that didn't even require strict ordering, in which case
	  pareto obj values could be potentially left as they are and not ranked.


Fitness determination methods:
	- Direct: fitness value is simply objective value
	- Ranking: simply order individuals by obj val and assign real valued fitness by rank


Selection methods:
	- Truncation: order population by fitness and take the best (requires only that fitness value is sortable)
	- Tournament: 
	- Roulette: proportional to fitness (requires fitness is real, so if obj not real, needs a fitness determination method that converts to real
*/

/*
ComparableFitness:
	The minimal requirement - two fitness values can be compared resulting in a winner and loser.

Must provide:
	operator < (RealFitness)		[Less than comparison]
*/

/*
SortableFitness:
	A fitness value type for which a strict ordering is defined.

Must provide:
	operator < (RealFitness)		[Less than comparison, must be strict ordering]
*/

/*
RealFitness:
	A real-number based fitness value. This is required for RouletteWheelSelection.

Must provide:
	operator < (RealFitness)		[Less than comparison, must be strict ordering]
	operator + (RealFitness)		[Sum fitness values]
	operator * (RealFitness)		[Product]
	operator / (RealFitness)		[Quotient]
*/

// mark_unfit()
// is_unfit()
// ?

struct basic_real_fitness
{
	double		value;

	// TODO: temp hack, if want this to be a member of fitness type, should be put into a base class
	bool		unfit;
	//

	basic_real_fitness(double _v = 0.0): value(_v), unfit(false)
	{}

	inline bool operator< (basic_real_fitness const& other) const
	{
		return value < other.value;
	}

	inline bool operator> (basic_real_fitness const& other) const
	{
		return value > other.value;
	}

	inline bool operator<= (basic_real_fitness const& other) const
	{
		return value <= other.value;
	}

	inline bool operator>= (basic_real_fitness const& other) const
	{
		return value >= other.value;
	}

	inline basic_real_fitness operator+ (basic_real_fitness const& other) const
	{
		return basic_real_fitness(value + other.value);
	}

	inline basic_real_fitness operator* (basic_real_fitness const& other) const
	{
		return basic_real_fitness(value * other.value);
	}

	inline basic_real_fitness operator/ (basic_real_fitness const& other) const
	{
		return basic_real_fitness(value / other.value);
	}

	inline basic_real_fitness operator* (double d) const
	{
		return basic_real_fitness(value * d);
	}

	inline basic_real_fitness operator/ (double d) const
	{
		return basic_real_fitness(value / d);
	}

	inline basic_real_fitness& operator+= (basic_real_fitness const& other)
	{
		value += other.value;
		return *this;
	}

	inline basic_real_fitness& operator*= (basic_real_fitness const& other)
	{
		value *= other.value;
		return *this;
	}

	inline basic_real_fitness& operator/= (basic_real_fitness const& other)
	{
		value /= other.value;
		return *this;
	}

	template < typename PopIterator >
	static std::string analyse_population_fitness(PopIterator pop_start, PopIterator pop_end)
	{
		double average = 0.0, best = -std::numeric_limits< double >::max();
		boost::iterator_range< PopIterator > range(pop_start, pop_end);
		for(auto const& ind: range)
		{
			double f = ind.fitness.value;
			average += f;
			if(f > best)
			{
				best = f;
			}
		}
		average /= range.size();

		std::stringstream ss;
		std::fixed(ss);
		ss.precision(4);

		ss << "Average = " << average << "; Best = " << best;

		return ss.str();
	}

	friend std::ostream& operator<< (std::ostream& out, basic_real_fitness const& gn);
};


inline std::ostream& operator<< (std::ostream& out, basic_real_fitness const& f)
{
	out.precision(4);
	std::fixed(out);

	out << f.value;

	return out;
}


#endif


