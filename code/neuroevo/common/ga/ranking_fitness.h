// ranking_fitness.h

#ifndef __RANKING_FITNESS_H
#define __RANKING_FITNESS_H

#include "fitness.h"


/*
Assigns fitness values to individuals by ranking them by their objective values.
*/
template <
	typename ProcessedObjValueType		// Must be LessThanComparable
>
struct ranking_fitness
{
	typedef ProcessedObjValueType	processed_obj_value_t;
	typedef basic_real_fitness		fitness_t;

	template < typename FitnessAccessor >
	struct ProcessedAccessorDefn
	{
		typedef FitnessAccessor		fitness_acc_t;

		struct obj_val_element
		{
			size_t						index;		// index into population collection
			processed_obj_value_t		obj_val;	// objective value for associated individual

			obj_val_element(): index(0), obj_val()
			{}

			inline bool operator< (obj_val_element const& other) const
			{
				return obj_val > other.obj_val;
			}
		};

		// Provide indexable access to the objective value of individuals
		// In this case, they are stored temporarily in a collection that will be sorted for the ranking.
		struct obj_val_accessor
		{
			std::vector< obj_val_element >	elems;

			obj_val_accessor(fitness_acc_t& _f, size_t const _pop_size)
			{
				elems.resize(_pop_size);
				for(size_t i = 0; i < elems.size(); ++i)
				{
					elems[i].index = i;
				}
			}

			inline processed_obj_value_t& operator[] (int idx)
			{
				return elems[idx].obj_val;
			}
		};

		typedef obj_val_accessor	processed_obj_val_acc_t;
	};

	template < typename FitnessAccessor >
	inline void operator() (typename ProcessedAccessorDefn< FitnessAccessor >::processed_obj_val_acc_t& obj_vals, FitnessAccessor& fitnesses) const
	{
		// Sort descending by processed objective value
		std::sort(obj_vals.elems.begin(), obj_vals.elems.end());

		// Now assign fitnesses based on rank
		size_t const pop_size = obj_vals.elems.size();
		double const SP = 1.5;	// TODO: Selection pressure parameter?
		for(size_t i = 0; i < pop_size; ++i)
		{
			size_t const idx = obj_vals.elems[i].index;
			size_t const inv_pos = pop_size - 1 - i;
			fitnesses[idx] = fitness_t(2.0 - SP + 2.0 * (SP - 1.0) * (inv_pos / (pop_size - 1)));
		}
	}

	template < typename FitnessAccessor >
	inline void reassign(typename ProcessedAccessorDefn< FitnessAccessor >::processed_obj_val_acc_t& obj_vals, FitnessAccessor& fitnesses) const
	{
		operator() (obj_vals, fitnesses);
	}
};


#endif


