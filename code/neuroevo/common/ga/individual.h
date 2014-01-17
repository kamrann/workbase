// individual.h

#ifndef __INDIVIDUAL_H
#define __INDIVIDUAL_H


template <
	typename Genome,
	typename ObjectiveValType,
	typename FitnessType
>
struct individual
{
	typedef Genome				genome_t;
	typedef ObjectiveValType	obj_value_t;
	typedef FitnessType			fitness_t;

	genome_t		genome;
	obj_value_t		obj_value;
	fitness_t		fitness;

	inline bool operator< (individual const& other) const
	{
		return other.fitness < fitness;
	}
};


#endif


